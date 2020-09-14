#pragma once

#include <string>
#include <tuple>
#include <optional>
#include "base_formula.h"
#include "atom.h"
#include "variable_term.h"

class Atom;
class VariableTerm;

class FormulaParser
{
public:
	FormulaParser(const std::string& formulaString);

	bool Success() const { return m_formula.has_value(); }
	Formula GetFormula() const { return m_formula.value(); }
	std::string GetError() const { return m_error; }

	static const AtomMap& GetParsedPredicates() { return m_parsedPredicates; }
	static const TermMap& GetParsedTerms() { return m_parsedTerms; }

private:
	/* Parses a single formula/variable/term from the input string */
	template <typename T>
	std::optional<T> ParseInternal(const std::string& str);

	/* Wrapper function --> parses the args string and converts/zips the arguments into a tuple expected by the caller */
	template<typename... Ts>
	std::optional<std::tuple<Ts...>> ParseAndConvertArgs(const std::string& argsString);

	/* Recursive templated function for converting a single argument */
	template<typename FTA, typename... OTA, typename = typename std::enable_if<sizeof...(OTA) != 0>::type>
	std::optional<std::tuple<FTA, OTA...>> ConvertArgs(std::vector<std::string>::const_iterator args);

	/* Base of the recursion for the previous function */
	template<typename FTA>
	std::optional<std::tuple<FTA>> ConvertArgs(std::vector<std::string>::const_iterator args);

	/* Parse all arguments by delimiting the string with commas */
	std::optional<std::vector<std::string>> ParseArgs(const std::string& argsString);

	std::string m_error;
	std::optional<Formula> m_formula;
	static AtomMap m_parsedPredicates;
	static TermMap m_parsedTerms;
};

template <> std::optional<Variable> FormulaParser::ParseInternal(const std::string& variableString);
template <> std::optional<Term> FormulaParser::ParseInternal(const std::string& termString);
template <> std::optional<Formula> FormulaParser::ParseInternal(const std::string& formulaString);