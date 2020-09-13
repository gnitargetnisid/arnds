#pragma once

#include <string>
#include <optional>
#include <vector>
#include "solver.h"

namespace ND
{
	class SolverParser
	{
	public:
		SolverParser(Solver& solver);
		bool Parse(const std::string& input);

		template <typename T>
		std::optional<T> ParseRuleInput(const std::string& input);

		const std::string& GetError() { return m_error; }

	private:
		enum UserCommand
		{
			Print,
			Add,
			Duplicate,
			List,
			Apply
		};

		std::optional<UserCommand> ReadCommand(const std::string& input, std::vector<std::string>& commandArguments);
		
		bool ParsePrint(const std::vector<std::string>& arguments);
		bool ParseAdd(const std::vector<std::string>& arguments);
		bool ParseDuplicate(const std::vector<std::string>& arguments);
		bool ParseList(const std::vector<std::string>& arguments);
		bool ParseApply(const std::vector<std::string>& arguments);

		template <typename T>
		bool CheckRuleAndApply(const std::string& ruleToCheck, const std::string& ruleCommand, const std::vector<std::string>& arguments, bool& success);

		Solver& m_solver;
		std::string m_error;
	};

	template <typename T, size_t N>
	struct RuleInputParser
	{
		static std::optional<Tuple<N, T>> Parse(SolverParser& parser, const std::vector<std::string>& inputs);
	};

	template <typename T>
	struct RuleInputParser<T, 0>
	{
		static std::optional<Tuple<0, T>> Parse(SolverParser& parser, const std::vector<std::string>& inputs) { return Tuple<0, T>(); }
	};
}