#include <memory>
#include <vector>
#include "first_order_logic/propositional_logic.h"
#include "types.h"

#pragma once

namespace ND
{
	class SolverTreeNodeBase;
	class SolverTreeInnerNodeBase;
	class SolverTreePremiseNodeBase;
	using SolverTreeNode = std::shared_ptr<SolverTreeNodeBase>;
	using SolverTreePremiseNode = std::shared_ptr<SolverTreePremiseNodeBase>;
	using PremiseInstance = SolverTreePremiseNode;

	class PremiseBase;
	using Premise = std::shared_ptr<PremiseBase>;

	class SolverTreeNodeBase : public std::enable_shared_from_this<SolverTreeNodeBase>
	{
		friend class SolverTree;

	public:
		void Resolve() { m_resolved = true; }
		
		Formula GetFormula() const { return m_formula; }
		ID GetID() const { return m_id; }
		bool GetResolved() const { return m_resolved; }
		ID GetOutputNodeID() const { return m_outputNode->GetID(); }

		template <typename T>
		bool IsFormulaA() { return m_formula->IsA<T>(); }

		template <typename T>
		bool IsA() { return dynamic_cast<T*>(this) != nullptr; }

		template <typename T>
		T* As() { return static_cast<T*>(this); }

		virtual ~SolverTreeNodeBase() {}

	private:
		Formula m_formula;
		ID m_id;
		bool m_resolved = false;

		std::vector<SolverTreeNode> m_inputNodes;
		SolverTreeNode m_outputNode;
	};

	class SolverTreePremiseNodeBase : public SolverTreeNodeBase
	{
		friend class SolveTree;

	public:
		void Eliminate() { m_eliminated = true; }
		void SetPremise(Premise premise) { m_premise = premise; }

		Premise GetPremise() const { return m_premise; }
		bool GetEliminated() const { return m_eliminated; }

	private:
		bool m_eliminated = false;
		Premise m_premise;
	};

	class SolverTree
	{
	public:
		ID AddPremiseNode(Premise premise);
		ID AddFormulaNode(const std::vector<SolverTreeNode>& parentNodes, Formula formula);

		const std::map<ID, SolverTreeNode>& GetNodes() const { return m_nodes; }
		SolverTreeNode GetNode(ID id) const { return m_nodes.at(id); }
		bool NodeExists(ID id) const { return m_nodes.find(id) != m_nodes.end(); }
		
		std::vector<SolverTreePremiseNode> GetAlivePremiseAncestors(ID node_id, ID premise_id) const;

	private:
		ID GetNextNodeID();
		void GetAlivePremiseAncestorsInternal(SolverTreeNode node_id, ID premise_id, std::vector<SolverTreePremiseNode>& premises) const;

		std::map<ID, SolverTreeNode> m_nodes;
		ID m_idCounter = 0;

	};

	class PremiseBase : public std::enable_shared_from_this<PremiseBase>
	{
	public:
		PremiseBase(ID id, Formula formula)
			: m_id(id)
			, m_formula(formula)
		{
		}

		void AddInstance(PremiseInstance instance) { m_instances.push_back(instance); }

		Formula GetFormula() const { return m_formula; }
		ID GetID() const { return m_id; }
		const std::vector<PremiseInstance>& GetInstances() const { return m_instances; }

	private:
		Formula m_formula;
		ID m_id;
		std::vector<PremiseInstance> m_instances;
	};
}