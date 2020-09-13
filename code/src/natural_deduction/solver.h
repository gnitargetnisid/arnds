#pragma once

#include "natural_deduction/solvertree.h"
#include "rules.h"

namespace ND
{
	class Solver
	{
	public:
		Solver();

		SolverTree& GetTree() { return m_tree; }
		const std::map<ID, SolverTreeNode>& GetTreeNodes() const { return m_tree.GetNodes(); }
		SolverTreeNode GetTreeNode(ID id) const { return m_tree.GetNode(id); }
		Formula GetFormula(ID id) const { return m_tree.GetNode(id)->GetFormula(); }
		bool FormulaExists(ID id) const { return m_tree.NodeExists(id); }

		std::tuple<ID, ID> AddPremise(Formula formula);
		ID DuplicatePremise(ID id);
		Premise GetPremise(ID id) const { return m_premises.at(id); }
		const std::map<ID, Premise>& GetPremises() const { return m_premises; }
		bool PremiseExists(ID id) const { return m_premises.find(id) != m_premises.end(); }

		bool ApplyRule(BaseRule& rule, std::string& error);

	private:
		ID GetNextPremiseID() { return ++m_premiseIDCounter; }

		SolverTree m_tree;
		std::map<ID, Premise> m_premises;

		ID m_premiseIDCounter = 0;
	};
}