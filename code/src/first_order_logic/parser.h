#pragma once

#include <string>
#include <tuple>
#include <optional>
#include "base_formula.h"
#include "atom.h"
#include "variable_term.h"

class Atom;
class VariableTerm;

template <typename T>
class Parser
{
public:
	Parser(const std::string& objectString);
	bool Success() { return m_object.has_value(); }
	T GetResult() const { return m_object.value(); }
	std::string GetError() const { return m_error; }

	virtual ~Parser() = default;

protected:
	std::string m_stringToParse;
	std::optional<T> m_object;
	std::string m_error;
};

class VariableParser : public Parser<Variable>
{
public:
	VariableParser(const std::string& variableString);

private:
	std::optional<Variable> ParseInternal();
};

class FormulaParser : public Parser<Formula>
{
public:
	FormulaParser(const std::string& formulaString);

	static const AtomMap& GetAllParsedAtoms() { return m_allParsedAtoms; }
	static const VariablesSet& GetAllParsedVariables() { return m_allParsedVariables; }

private:
	/* Parses a single formula/term from the input string */
	template <typename T>
	std::optional<T> ParseInternal(const std::string& stringToParse);

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

	std::optional<Formula> TryMakeAtom(const RelationSymbol& symbol, const std::vector<Term>& terms);

	bool VariableExists(const Variable& var) const;

	bool RelationSymbolExists(const RelationSymbol& symbol, Arity& arity) const;

	static AtomMap m_allParsedAtoms;
	static VariablesSet m_allParsedVariables;
	AtomMap m_parsedAtoms;
	VariablesSet m_parsedVariables;
};


template <> std::optional<Variable> FormulaParser::ParseInternal(const std::string& variableString);
template <> std::optional<Term> FormulaParser::ParseInternal(const std::string& termString);
template <> std::optional<Formula> FormulaParser::ParseInternal(const std::string& formulaString);