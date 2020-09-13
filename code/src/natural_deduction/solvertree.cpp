#include "solvertree.h"
#include <assert.h>

namespace ND
{
	ID SolverTree::AddPremiseNode(Premise premise)
	{
		SolverTreePremiseNode node = std::make_shared<SolverTreePremiseNodeBase>();
		node->m_formula = premise->GetFormula();
		node->m_id = GetNextNodeID();
		node->SetPremise(premise);	//wtf here --> friend class can't access private member??????
		node->m_resolved = false;
		m_nodes.insert({ node->m_id, node });

		premise->AddInstance(node);
		return node->m_id;
	}

	ID SolverTree::AddFormulaNode(const std::vector<SolverTreeNode>& parentNodes, Formula formula)
	{
		SolverTreeNode node = std::make_shared<SolverTreeNodeBase>();
		node->m_formula = formula;
		node->m_id = GetNextNodeID();
		node->m_resolved = false;
		node->m_inputNodes = parentNodes;
		m_nodes.insert({ node->m_id, node });

		for (auto& parentNode : parentNodes)
		{
			assert(parentNode->m_resolved);
			parentNode->m_outputNode = node;
		}

		return node->m_id;
	}

	std::vector<SolverTreePremiseNode> SolverTree::GetAlivePremiseAncestors(ID node_id, ID premise_id) const
	{
		std::vector<SolverTreePremiseNode> premises;
		SolverTreeNode beginNode = m_nodes.at(node_id);
		GetAlivePremiseAncestorsInternal(beginNode, premise_id, premises);
		return premises;
	}

	void SolverTree::GetAlivePremiseAncestorsInternal(SolverTreeNode node, ID premise_id, std::vector<SolverTreePremiseNode>& premises) const
	{
		if (node->m_inputNodes.size() == 0)
		{
			SolverTreePremiseNode premise = std::dynamic_pointer_cast<SolverTreePremiseNodeBase>(node);
			if (!premise->GetEliminated() && premise->GetPremise()->GetID() == premise_id)
			{
				premises.push_back(premise);
			}

			return;
		}

		for (auto& ancestorNode : node->m_inputNodes)
		{
			GetAlivePremiseAncestorsInternal(ancestorNode, premise_id, premises);
		}
	}

	ID SolverTree::GetNextNodeID()
	{
		return ++m_idCounter;
	}
}