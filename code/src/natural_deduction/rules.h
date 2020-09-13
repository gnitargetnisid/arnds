#pragma once

#include "first_order_logic/propositional_logic.h"
#include "solvertree.h"
#include <cassert>
#include <optional>

namespace ND
{
	class BaseRule
	{
	public:
		virtual std::optional<Formula> Apply() = 0;
		virtual std::vector<SolverTreeNode> GetInputNodes() const = 0;
		virtual const std::vector<SolverTreePremiseNode>& GetEliminatedPremiseNodes() const = 0;
		virtual const std::string& GetError() const = 0;
	};

	template <size_t N, size_t offset = 0>
	struct RuleHelper
	{
		static void ResolveInputNodes(Tuple<N, SolverTreeNode>& nodes)
		{
			std::get<offset>(nodes)->Resolve();
			RuleHelper<N, offset + 1>::ResolveInputNodes(nodes);
		}
	};

	template <size_t N>
	struct RuleHelper<N, N>
	{
		static void ResolveInputNodes(Tuple<N, SolverTreeNode>& nodes)
		{
		}
	};

	template <size_t n_input_nodes, size_t n_premises, size_t n_formulas>
	class Rule : public BaseRule
	{
	public:
		Rule(const SolverTree& tree, Tuple<n_input_nodes, SolverTreeNode> nodes, Tuple<n_premises, Premise> premises, Tuple<n_formulas, Formula> formulas)
			: m_tree(tree)
			, m_inputNodes(nodes)
			, m_premises(premises)
			, m_formulas(formulas)
		{
		}

		virtual std::optional<Formula> ApplyInternal() = 0;
		std::optional<Formula> Apply() override
		{
			std::optional<Formula> newFormula = ApplyInternal();
			if (newFormula.has_value())
			{
				ResolveInputNodes();
			}
			return newFormula;
		}

		std::vector<SolverTreeNode> GetInputNodes() const override { return ConvertTupleToVector<n_input_nodes, SolverTreeNode>::Convert(m_inputNodes); }
		const std::vector<SolverTreePremiseNode>& GetEliminatedPremiseNodes() const override { return m_eliminatedPremiseNodes; }
		const std::string& GetError() const override { return m_error; }

		static constexpr size_t GetRequiredNodeArgCount() { return n_input_nodes; }
		static constexpr size_t GetRequiredPremiseArgCount() { return n_premises; }
		static constexpr size_t GetRequiredFormulaArgCount() { return n_formulas; }

	protected:
		void SetError(const std::string& error) { m_error = error; }

		template <size_t i>
		SolverTreeNode GetInputNode() const { return std::get<i>(m_inputNodes); }
		template <size_t i>
		Premise GetPremise() const { return std::get<i>(m_premises); }
		template <size_t i>
		Formula GetFormula() const { return std::get<i>(m_formulas); }

		void EliminatePremiseNode(SolverTreePremiseNode node)
		{
			node->Eliminate();
			m_eliminatedPremiseNodes.push_back(node);
		}

		const SolverTree& m_tree;

	private:
		void ResolveInputNodes()
		{
			RuleHelper<n_input_nodes>::ResolveInputNodes(m_inputNodes);
		}

		Tuple<n_input_nodes, SolverTreeNode> m_inputNodes;
		Tuple<n_premises, Premise> m_premises;
		Tuple<n_formulas, Formula> m_formulas;

		std::vector<SolverTreePremiseNode> m_eliminatedPremiseNodes;
		std::string m_error;
	};

	template <size_t n_input_nodes>
	class BasicRule : public Rule<n_input_nodes, 0, 0>
	{
	public:
		using Rule<n_input_nodes, 0, 0>::Rule;
	};

	template <size_t n_input_nodes, size_t n_premises>
	class PremiseEliminationRule : public Rule<n_input_nodes, n_premises, 0>
	{
	public:
		using Rule<n_input_nodes, n_premises, 0>::Rule;
	};

	template <size_t n_input_nodes, size_t n_formulas>
	class FormulaIntroductionRule : public Rule<n_input_nodes, 0, n_formulas>
	{
	public:
		using Rule<n_input_nodes, 0, n_formulas>::Rule;
	};

	class NotIntroduction : public PremiseEliminationRule<1, 1>
	{
	public:
		using PremiseEliminationRule::PremiseEliminationRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class NotElimination : public BasicRule<2>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ConjunctionIntroduction : public BasicRule<2>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ConjunctionEliminationLHS : public BasicRule<1>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ConjunctionEliminationRHS : public BasicRule<1>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class DisjunctionIntroductionLHS : public FormulaIntroductionRule<1, 1>
	{
	public:
		using FormulaIntroductionRule::FormulaIntroductionRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class DisjunctionIntroductionRHS : public FormulaIntroductionRule<1, 1>
	{
	public:
		using FormulaIntroductionRule::FormulaIntroductionRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class DisjunctionElimination : public PremiseEliminationRule<3, 2>
	{
	public:
		using PremiseEliminationRule::PremiseEliminationRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ImplicationIntroduction : public PremiseEliminationRule<1, 1>
	{
	public:
		using PremiseEliminationRule::PremiseEliminationRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ImplicationElimination : public BasicRule<2>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ContradictionElimination : public FormulaIntroductionRule<1, 1>
	{
	public:
		using FormulaIntroductionRule::FormulaIntroductionRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class TrueIntroduction : public BasicRule<0>
	{
	public:
		using BasicRule::BasicRule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ForallIntroduction : public Rule<1, 0, 2>
	{
	public:
		using Rule::Rule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ForallElimination : public Rule<1, 0, 1>
	{
	public:
		using Rule::Rule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ExistsIntroduction : public Rule<1, 0, 2>
	{
	public:
		using Rule::Rule;
		std::optional<Formula> ApplyInternal() override;
	};

	class ExistsElimination : public Rule<2, 1, 1>
	{
	public:
		using Rule::Rule;
		std::optional<Formula> ApplyInternal() override;
	};
}