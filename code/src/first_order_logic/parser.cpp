#include "parser.h"

#include <algorithm>
#include "variable_term.h"
#include "propositional_logic.h"
#include "exists.h"
#include "forall.h"

FormulaParser::FormulaParser(const std::string& formulaString)
{
	//Preprocessing
	std::string stringToProcess = formulaString;
	stringToProcess.erase(std::remove_if(stringToProcess.begin(), stringToProcess.end(), std::isspace), stringToProcess.end());
	std::transform(stringToProcess.begin(), stringToProcess.end(), stringToProcess.begin(), [](auto & c) { return std::tolower(c); });
	//And conversion
	m_formula = ParseInternal<Formula>(stringToProcess);
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
			auto atom = std::make_shared<Atom>(formulaString, std::vector<Term>());
			//TO-DO: INSERT ERROR CHECKING FOR ARITY OF THE ATOM
			m_parsedPredicates[formulaString].push_back(atom);
			return atom;
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
		if (std::all_of(fn_name.begin(), fn_name.end(), std::isalnum))
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
				auto atom = std::make_shared<Atom>(fn_name, atomTerms);
				m_parsedPredicates[fn_name].push_back(atom);
				return atom;
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

template <>
std::optional<Term> FormulaParser::ParseInternal<Term>(const std::string& termString)
{
	//No support for function terms
	auto variable = ParseInternal<Variable>(termString);
	if (variable.has_value())
	{
		auto term = std::make_shared<VariableTerm>(variable.value());
		m_parsedTerms[termString].push_back(term);
		return term;
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