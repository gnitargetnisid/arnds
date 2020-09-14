#include "parser.h"

#include <algorithm>
#include <cctype>

#include "variable_term.h"
#include "propositional_logic.h"
#include "exists.h"
#include "forall.h"

VariablesSet FormulaParser::m_allParsedVariables = VariablesSet();
AtomMap FormulaParser::m_allParsedAtoms = AtomMap();

template <typename T>
Parser<T>::Parser(const std::string& objectString)
	: m_stringToParse(objectString)
{
	//Preprocessing
	m_stringToParse.erase(std::remove_if(m_stringToParse.begin(), m_stringToParse.end(), [](auto & c) { return std::isspace(c); }), m_stringToParse.end());
	std::transform(m_stringToParse.begin(), m_stringToParse.end(), m_stringToParse.begin(), [](auto & c) { return std::tolower(c); });
}

VariableParser::VariableParser(const std::string& variableString)
	: Parser(variableString)
{
	m_object = ParseInternal();
}

std::optional<Variable> VariableParser::ParseInternal()
{
	std::string& variableString = m_stringToParse;
	if (std::all_of(variableString.begin(), variableString.end(), isalnum))
	{
		return variableString;
	}
	else
	{
		m_error = "Found a non-alphanumeric string where a variable is expected. String: " + variableString;
		return std::nullopt;
	}
}

FormulaParser::FormulaParser(const std::string& formulaString)
	: Parser(formulaString)
{
	m_object = ParseInternal<Formula>(m_stringToParse);

	if (Success())
	{
		for (const auto& atomIt : m_parsedAtoms)
		{
			RelationSymbol symbol = atomIt.first;

			auto& persistentAtomIt = m_allParsedAtoms[symbol];

			for (const auto& atom : std::get<0>(atomIt.second))
			{
				std::get<0>(persistentAtomIt).push_back(atom);
			}
			std::get<1>(persistentAtomIt) = std::get<1>(atomIt.second);
		}

		for (const auto& variable : m_parsedVariables)
		{
			m_allParsedVariables.insert(variable);
		}
	}

}

template <>
std::optional<Formula> FormulaParser::ParseInternal<Formula>(const std::string& formulaString)
{
	//Special case: constants or atoms with no terms (predicates)
	if (formulaString.size() == 1)
	{
		switch (formulaString[0])
		{
		case 'T':
			return std::make_shared<True>();
		case 'F':
			return std::make_shared<False>();
		default:
			return TryMakeAtom(formulaString, std::vector<Term>());
		}
	}

	size_t opening_bracket_index = formulaString.find_first_of('(');
	size_t closing_bracket_index = formulaString.find_last_of(')');

	if (opening_bracket_index >= closing_bracket_index)
	{
		//ERROR
		return std::nullopt;
	}

	std::string fn_name = formulaString.substr(0, opening_bracket_index);
	std::string args_string = formulaString.substr(opening_bracket_index + 1, closing_bracket_index - opening_bracket_index - 1);

	//Now we decide what to do depending on the function name
	if (fn_name == "not")
	{
		auto args = ParseAndConvertArgs<Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Not>(std::get<0>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "or")
	{
		auto args = ParseAndConvertArgs<Formula, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Or>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "and")
	{
		auto args = ParseAndConvertArgs<Formula, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<And>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "imp")
	{
		auto args = ParseAndConvertArgs<Formula, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Imp>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "iff")
	{
		auto args = ParseAndConvertArgs<Formula, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Iff>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "forall")
	{
		//technically, the first param is not a term but a variable, but as far as parsing goes we don't really care
		auto args = ParseAndConvertArgs<Variable, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Forall>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else if (fn_name == "exists")
	{
		auto args = ParseAndConvertArgs<Variable, Formula>(args_string);
		return args.has_value() ? std::optional(std::make_shared<Exists>(std::get<0>(args.value()), std::get<1>(args.value()))) : std::nullopt;
	}
	else
	{
		if (std::all_of(fn_name.begin(), fn_name.end(), [](auto & c) { return std::isalnum(c); }))
		{
			auto args = ParseArgs(args_string);
			if (args.has_value())
			{
				std::vector<Term> atomTerms;
				atomTerms.reserve(args.value().size());
				for (auto& arg : args.value())
				{
					auto parsedArg = ParseInternal<Term>(arg);
					if (parsedArg.has_value())
					{
						atomTerms.push_back(parsedArg.value());
					}
					else
					{
						return std::nullopt;
					}
				}

				return TryMakeAtom(fn_name, atomTerms);
			}
			else
			{
				return std::nullopt;
			}
		}
		else
		{
			//ADD ERROR HERE
			return std::nullopt;
		}
	}
}

template <>
std::optional<Variable> FormulaParser::ParseInternal<Variable>(const std::string& variableString)
{
	Arity unusedArity;
	if (RelationSymbolExists(variableString, unusedArity))
	{
		m_error = "Symbol " + variableString + " already exists, but as a relation symbol. Unable to parse it as a term.";
		return std::nullopt;
	}

	VariableParser varParser(variableString);
	if (varParser.Success())
	{
		Variable parsedVariable = varParser.GetResult();
		m_parsedVariables.insert(parsedVariable);
		return parsedVariable;
	}
	else
	{
		m_error = varParser.GetError();
		return std::nullopt;
	}
}

template <>
std::optional<Term> FormulaParser::ParseInternal<Term>(const std::string& termString)
{
	auto variable = ParseInternal<Variable>(termString);
	if (variable.has_value())
	{
		return std::make_shared<VariableTerm>(variable.value());
	}
	else
	{
		return std::nullopt;
	}
}

template<typename... Ts>
std::optional<std::tuple<Ts...>> FormulaParser::ParseAndConvertArgs(const std::string& argsString)
{
	auto parsedArgs = ParseArgs(argsString);
	if (!parsedArgs.has_value())
	{
		return std::nullopt;
	}

	if (parsedArgs.value().size() != sizeof...(Ts))
	{
		m_error = "Error: expected " + std::to_string(sizeof...(Ts)) + " arguments, but got " + std::to_string(parsedArgs.value().size()) + " instead. String: " + argsString;
		return std::nullopt;
	}

	return ConvertArgs<Ts...>(parsedArgs.value().cbegin());
}

template<typename FTA, typename... OTA, typename>
std::optional<std::tuple<FTA, OTA...>> FormulaParser::ConvertArgs(std::vector<std::string>::const_iterator args)
{
	std::optional<FTA> arg = ParseInternal<FTA>(*args);
	if (arg.has_value())
	{
		auto convertedArgs = ConvertArgs<OTA...>(args + 1);
		if (convertedArgs.has_value())
		{
			return std::tuple_cat<std::tuple<FTA>, std::tuple<OTA>...>(std::tuple<FTA>(arg.value()), std::move(convertedArgs.value()));
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

template<typename FTA>
std::optional<std::tuple<FTA>> FormulaParser::ConvertArgs(std::vector<std::string>::const_iterator args)
{
	std::optional<FTA> arg = ParseInternal<FTA>(*args);
	if (arg.has_value())
	{
		return std::tuple<FTA>(arg.value());
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<std::vector<std::string>> FormulaParser::ParseArgs(const std::string& argsString)
{
	std::vector<std::string> retnArgs;
	std::string currentArg = "";
	unsigned bracketState = 0;
	size_t lastSplitPosition = 0;
	for (auto& character : argsString)
	{
		switch (character)
		{
		case '(':
			++bracketState;
			break;
		case ')':
			if (bracketState == 0)
			{
				m_error = "Error: closed bracket before opened bracket in " + argsString;
				return std::nullopt;
			}
			--bracketState;
			break;
		case ',':
			if (bracketState == 0)
			{
				retnArgs.push_back(currentArg);
				currentArg = "";
				continue;
			}
		}

		currentArg += character;
	}

	if (bracketState > 0)
	{
		m_error = "Error: you're missing some closed brackets in " + argsString;
		return std::nullopt;
	}

	retnArgs.push_back(currentArg);

	return retnArgs;
}

std::optional<Formula> FormulaParser::TryMakeAtom(const RelationSymbol& symbol, const std::vector<Term>& terms)
{
	if (VariableExists(symbol))
	{
		m_error = "Cannot use " + symbol + " as a relation symbol because there's already a term by the same name.";
		return std::nullopt;
	}

	Arity existingRelationSymbolArity;
	if (RelationSymbolExists(symbol, existingRelationSymbolArity) && existingRelationSymbolArity != terms.size())
	{
		m_error = "Relation symbol " + symbol + " already exists, but with different arity.";
		return std::nullopt;
	}

	auto atom = std::make_shared<Atom>(symbol, terms);
	auto& atomIt = m_parsedAtoms[symbol];
	std::get<0>(atomIt).push_back(atom);
	std::get<1>(atomIt) = terms.size();
	return atom;
}

bool FormulaParser::VariableExists(const Variable& var) const
{
	return m_allParsedVariables.find(var) != m_allParsedVariables.end() || m_parsedVariables.find(var) != m_parsedVariables.end();
}

bool FormulaParser::RelationSymbolExists(const RelationSymbol& symbol, Arity& arity) const
{
	auto persistentSymbolIt = m_allParsedAtoms.find(symbol);
	if (persistentSymbolIt != m_allParsedAtoms.end())
	{
		arity = std::get<1>(persistentSymbolIt->second);
		return true;
	}

	auto temporarySymbolIt = m_parsedAtoms.find(symbol);
	if (temporarySymbolIt != m_parsedAtoms.end())
	{
		arity = std::get<1>(temporarySymbolIt->second);
		return true;
	}

	return false;
}