#include "solverparser.h"
#include <algorithm>
#include <vector>
#include <sstream>
#include "types.h"
#include "first_order_logic/parser.h"
#include "rules.h"

namespace ND
{
	SolverParser::SolverParser(Solver& solver)
		: m_solver(solver)
	{
	}

	bool SolverParser::Parse(const std::string& input)
	{
		m_error = "";
		std::string inputProcessed = input;
		inputProcessed.erase(std::remove_if(inputProcessed.begin(), inputProcessed.end(), std::isspace), inputProcessed.end());
		std::transform(inputProcessed.begin(), inputProcessed.end(), inputProcessed.begin(), [](auto & c) { return std::tolower(c); });

		std::vector<std::string> commandArguments;
		auto command = ReadCommand(inputProcessed, commandArguments);

		if (!command.has_value())
		{
			return false;
		}

		switch (command.value())
		{
		case UserCommand::Print:
			return ParsePrint(commandArguments);
		case UserCommand::Add:
			return ParseAdd(commandArguments);
		case UserCommand::Duplicate:
			return ParseDuplicate(commandArguments);
		case UserCommand::List:
			return ParseList(commandArguments);
		case UserCommand::Apply:
			return ParseApply(commandArguments);
		default:
			return false;
		}
	}

	std::optional<SolverParser::UserCommand> SolverParser::ReadCommand(const std::string& input, std::vector<std::string>& commandArguments)
	{
		std::stringstream inputStream(input);
		bool foundFirst = false;
		std::string word;
		std::string command;
		while (std::getline(inputStream, word, '|'))
		{
			if (!foundFirst)
			{
				foundFirst = true;
				command = word;
			}
			else
			{
				commandArguments.push_back(word);
			}
		}

		if (!foundFirst)
		{
			m_error = "Sorry, couldn't understand.";
			return std::nullopt;
		}

		if (command == "print")
		{
			return UserCommand::Print;
		}
		else if (command == "add")
		{
			return UserCommand::Add;
		}
		else if (command == "duplicate")
		{
			return UserCommand::Duplicate;
		}
		else if (command == "list")
		{
			return UserCommand::List;
		}
		else if (command == "apply")
		{
			return UserCommand::Apply;
		}

		m_error = "Invalid command.";
		return std::nullopt;
	}

	bool SolverParser::ParsePrint(const std::vector<std::string>& arguments)
	{
		if (arguments.size() != 2)
		{
			m_error = "Invalid amount of arguments for Print. Expected two arguments: P/F and ID";
			return false;
		}

		const std::string nodeTypeStr = arguments[0];
		const std::string nodeIDStr = arguments[1];

		if (nodeTypeStr.size() != 1)
		{
			m_error = "Invalid first argument for Print. Expected P or F.";
			return false;
		}

		ID id = 0;
		if (!ConvertFromString(nodeIDStr, id))
		{
			m_error = "Invalid second argument for Print. Expected ID of the premise or formula.";
			return false;
		}

		switch (nodeTypeStr[0])
		{
		case 'p':
		{
			if (!m_solver.PremiseExists(id))
			{
				m_error = "Invalid second argument for Print. Expected a valid premise ID.";
				return false;
			}

			Premise premise = m_solver.GetPremise(id);
			std::cout << "Premise [" + std::to_string(id) + "]: " + premise->GetFormula()->getText() << std::endl;
			return true;
		}
		case 'f':
		{
			if (!m_solver.FormulaExists(id))
			{
				m_error = "Invalid second argument for Print. Expected a valid formula ID.";
				return false;
			}

			Formula formula = m_solver.GetFormula(id);
			std::cout << "Formula [" + std::to_string(id) + "]: " + formula->getText() << std::endl;
			return true;
		}
		default:
		{
			m_error = "Invalid first argument for Print. Expected P or F.";
			return false;
		}
		}

		return false;
	}

	bool SolverParser::ParseAdd(const std::vector<std::string>& arguments)
	{
		if (arguments.size() != 1)
		{
			m_error = "Add expects 1 argument, you've provided " + std::to_string(arguments.size());
			return false;
		}

		const std::string formulaStr = arguments[0];
		FormulaParser parser(formulaStr);

		if (parser.Success())
		{
			Formula parsedFormula = parser.GetFormula();
			std::tuple<ID, ID> premiseIDs = m_solver.AddPremise(parsedFormula);
			std::cout << "Premise [" + std::to_string(std::get<0>(premiseIDs)) + "] added. Premise instance [" + std::to_string(std::get<1>(premiseIDs)) + "] added. Formula: " << parsedFormula->getText() << std::endl;
			return true;
		}
		else
		{
			m_error = parser.GetError();
			return false;
		}
	}

	bool SolverParser::ParseDuplicate(const std::vector<std::string>& arguments)
	{
		if (arguments.size() != 1)
		{
			m_error = "Duplicate expects 1 argument, you've provided " + std::to_string(arguments.size());
			return false;
		}

		const std::string premiseIDStr = arguments[0];
		ID id = 0;
		if (!ConvertFromString(premiseIDStr, id) || !m_solver.PremiseExists(id))
		{
			m_error = "Invalid argument for Duplicate. Expected ID of a premise.";
			return false;
		}
		
		ID newID = m_solver.DuplicatePremise(id);
		std::cout << "Duplicate of Premise [" + std::to_string(id) + "] added under ID [" + std::to_string(newID) + "]" << std::endl;
		return true;
	}

	bool SolverParser::ParseList(const std::vector<std::string>& arguments)
	{
		if (arguments.size() != 1)
		{
			m_error = "List expects 1 argument, you've provided " + std::to_string(arguments.size());
			return false;
		}

		const std::string listTypeString = arguments[0];

		if (listTypeString == "p")
		{
			//All premises and their status
			std::cout << "All premises: " << std::endl;
			const auto& allPremises = m_solver.GetPremises();
			for (const auto& premiseInfo : allPremises)
			{
				ID id = premiseInfo.first;
				Premise premise = premiseInfo.second;
				for (const auto& instance : premise->GetInstances())
				{
					std::cout << "Premise [" << id << "] Instance [" << instance->GetID() << "] Formula: " << premise->GetFormula()->getText() << " ---- " << (instance->GetEliminated() ? "Eliminated" : "Pending") << std::endl;
				}
			}
		}
		else if (listTypeString == "pp")
		{
			//Pending premises
			std::cout << "Pending premises: " << std::endl;
			const auto& allPremises = m_solver.GetPremises();
			for (const auto& premiseInfo : allPremises)
			{
				ID id = premiseInfo.first;
				Premise premise = premiseInfo.second;
				for (const auto& instance : premise->GetInstances())
				{
					if (!instance->GetEliminated())
					{
						std::cout << "Premise [" << id << "] Instance [" << instance->GetID() << "] " << premise->GetFormula()->getText() << std::endl;
					}
				}
			}
		}
		else if (listTypeString == "f")
		{
			//All formulas and their status
			std::cout << "All formulas: " << std::endl;
			const auto& allTreeNodes = m_solver.GetTreeNodes();
			for (const auto& treeNodeInfo : allTreeNodes)
			{
				ID id = treeNodeInfo.first;
				SolverTreeNode node = treeNodeInfo.second;
				std::cout << "Formula [" << id << "] " << node->GetFormula()->getText() << " ---- " << (node->GetResolved() ? "Resolved ----> [" + std::to_string(node->GetOutputNodeID()) + "]" : "Pending") << std::endl;
			}
		}
		else if (listTypeString == "fp")
		{
			//Pending formulas
			std::cout << "Pending formulas: " << std::endl;
			const auto& allTreeNodes = m_solver.GetTreeNodes();
			for (const auto& treeNodeInfo : allTreeNodes)
			{
				ID id = treeNodeInfo.first;
				SolverTreeNode node = treeNodeInfo.second;
				if (!node->GetResolved())
				{
					std::cout << "Formula [" << id << "] " << node->GetFormula()->getText() << std::endl;
				}
			}
		}
		else
		{
			m_error = "Expected one of {'P', 'PP', 'F', 'FP'} as the first argument to List.";
			return false;
		}

		return true;
	}

	bool SolverParser::ParseApply(const std::vector<std::string>& arguments)
	{
		if (arguments.size() == 0)
		{
			m_error = "Apply expects at least 1 argument, you've provided 0";
			return false;
		}

		const std::string ruleString = arguments[0];

		std::vector<std::string> ruleArguments;
		for (auto it = arguments.cbegin() + 1; it != arguments.cend(); ++it)
		{
			ruleArguments.push_back(*it);
		}

		bool success = false;

		if (CheckRuleAndApply<NotIntroduction>("in", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<NotElimination>("en", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ConjunctionIntroduction>("ic", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ConjunctionEliminationLHS>("elc", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ConjunctionEliminationRHS>("erc", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<DisjunctionIntroductionLHS>("ild", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<DisjunctionIntroductionRHS>("ird", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<DisjunctionElimination>("ed", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ImplicationIntroduction>("ii", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ImplicationElimination>("ei", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<TrueIntroduction>("it", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ContradictionElimination>("ec", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ForallIntroduction>("if", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ForallElimination>("ef", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ExistsIntroduction>("ie", ruleString, ruleArguments, success)) return success;
		if (CheckRuleAndApply<ExistsElimination>("ee", ruleString, ruleArguments, success)) return success;
		
		m_error = "Unsupported apply rule.";
	    return false;
	}

	template <typename T>
	bool SolverParser::CheckRuleAndApply(const std::string& ruleToCheck, const std::string& ruleCommand, const std::vector<std::string>& arguments, bool& success)
	{
		//Check whether this is the rule that the user requested
		if (ruleToCheck != ruleCommand)
		{
			return false;
		}

		size_t ruleArgCount = arguments.size();
		constexpr size_t expectedNodeArgCount = T::GetRequiredNodeArgCount();
		constexpr size_t expectedPremiseArgCount = T::GetRequiredPremiseArgCount();
		constexpr size_t expectedFormulaArgCount = T::GetRequiredFormulaArgCount();
		size_t expectedArgCount = expectedNodeArgCount + expectedPremiseArgCount + expectedFormulaArgCount;

		if (expectedArgCount == ruleArgCount)
		{
			auto GetSubarguments = [arguments](size_t offset, size_t count)
			{
				std::vector<std::string> subarguments;
				for (size_t i = 0; i < count; ++i)
				{
					subarguments.push_back(arguments[i + offset]);
				}
				return subarguments;
			};

			std::vector<std::string> nodeInputs = GetSubarguments(0, expectedNodeArgCount);
			std::vector<std::string> premiseInputs = GetSubarguments(expectedNodeArgCount, expectedPremiseArgCount);
			std::vector<std::string> formulaInputs = GetSubarguments(expectedNodeArgCount + expectedPremiseArgCount, expectedFormulaArgCount);

			auto argNodes = RuleInputParser<SolverTreeNode, expectedNodeArgCount>::Parse(*this, nodeInputs);
			auto argPremises = RuleInputParser<Premise, expectedPremiseArgCount>::Parse(*this, premiseInputs);
			auto argFormulas = RuleInputParser<Formula, expectedFormulaArgCount>::Parse(*this, formulaInputs);

			if (argNodes.has_value() && argPremises.has_value() && argFormulas.has_value())
			{
				T rule(m_solver.GetTree(), argNodes.value(), argPremises.value(), argFormulas.value());
				success = m_solver.ApplyRule(rule, m_error);
			}
			else
			{
				success = false;
			}
		}
		else
		{
			m_error = "Invalid amount of arguments. The rule you've requested expects " + std::to_string(expectedArgCount) + " arguments.";
			success = false;
		}

		return true;
	}

	template <>
	std::optional<SolverTreeNode> SolverParser::ParseRuleInput(const std::string& input)
	{
		ID id = 0;
		if (ConvertFromString(input, id))
		{
			if (!m_solver.FormulaExists(id))
			{
				m_error = "Formula/Node with ID " + input + " doesn't exist. Please provide a valid ID.";
				return std::nullopt;
			}

			SolverTreeNode node = m_solver.GetTreeNode(id);
			if (node->GetResolved())
			{
				m_error = "Formula/Node [" + std::to_string(id) + "] has already been resolved. You can't apply rules on resolved formulas/nodes.";
				return std::nullopt;
			}

			return node;
		}
		else
		{
			m_error = "Failed to parse the formula ID you provided.";
			return std::nullopt;
		}
	}

	template <>
	std::optional<Premise> SolverParser::ParseRuleInput(const std::string& input)
	{
		ID id = 0;
		if (ConvertFromString(input, id))
		{
			if (!m_solver.PremiseExists(id))
			{
				m_error = "Premise with ID " + input + " doesn't exist. Please provide a valid ID.";
				return std::nullopt;
			}

			return m_solver.GetPremise(id);
		}
		else
		{
			m_error = "Failed to parse the premise ID you provided.";
			return std::nullopt;
		}
	}

	template <>
	std::optional<Formula> SolverParser::ParseRuleInput(const std::string& input)
	{
		FormulaParser formulaParser(input);
		if (formulaParser.Success())
		{
			return formulaParser.GetFormula();
		}
		else
		{
			m_error = "Failed to parse the formula you provided.";
			return std::nullopt;
		}
	}

	template <typename T, size_t N>
	std::optional<Tuple<N, T>> RuleInputParser<T, N>::Parse(SolverParser& parser, const std::vector<std::string>& inputs)
	{
		assert(inputs.size() >= N);

		size_t inputIndex = inputs.size() - N;
		auto parsedInput = parser.ParseRuleInput<T>(inputs[inputIndex]);
		if (parsedInput.has_value())
		{
			auto restParsed = RuleInputParser<T, N - 1>::Parse(parser, inputs);
			if (restParsed.has_value())
			{
				return std::tuple_cat(std::move(std::tuple<T>(parsedInput.value())), std::move(restParsed.value()));
			}
			else
			{
				return std::nullopt;
			}
		}
		else
		{
			return std::nullopt;
		}
	}
}