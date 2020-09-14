#include "solver.h"
#include "rules.h"

namespace ND
{
	Solver::Solver()
	{
	}

	std::tuple<ID, ID> Solver::AddPremise(Formula formula)
	{
		Premise premise = std::make_shared<PremiseBase>(GetNextPremiseID(), formula);
		m_premises.insert({ premise->GetID(), premise });
		return std::tuple<ID, ID>(premise->GetID(), m_tree.AddPremiseNode(premise));
	}

	ID Solver::DuplicatePremise(ID id)
	{
		Premise premise = m_premises[id];
		ID newID = m_tree.AddPremiseNode(premise);
		return newID;
	}

	bool Solver::GetAllPremisesEliminated() const
	{
		for (const auto& premiseIt : m_premises)
		{
			for (const auto& premiseInstance : premiseIt.second->GetInstances())
			{
				if (!premiseInstance->GetEliminated())
				{
					return false;
				}
			}
		}

		return true;
	}

	std::optional<std::vector<Formula>> Solver::GetDerivedFormulas() const
	{
		if (!GetAllPremisesEliminated())
		{
			return std::nullopt;
		}

		std::vector<Formula> pendingFormulas;
		for (const auto& formulaNode : m_tree.GetNodes())
		{
			if (!formulaNode.second->GetResolved())
			{
				pendingFormulas.push_back(formulaNode.second->GetFormula());
			}
		}

		return pendingFormulas;
	}

	bool Solver::ApplyRule(BaseRule& rule, std::string& error)
	{
		auto ruleFormula = rule.Apply();
		if (ruleFormula.has_value())
		{
			const auto& inputNodes = rule.GetInputNodes();
			ID formulaNodeID = m_tree.AddFormulaNode(inputNodes, ruleFormula.value());

			std::cout << "New Formula [" + std::to_string(formulaNodeID) + "]: " + ruleFormula.value()->getText() << std::endl;

			if (!inputNodes.empty())
			{
				std::cout << "Resolved nodes: ";
				for (const auto& resolvedNode : inputNodes)
				{
					std::cout << "[" << resolvedNode->GetID() << "]";
				}
				std::cout << std::endl;
			}
			
			const auto& eliminatedPremiseNodes = rule.GetEliminatedPremiseNodes();
			if (!eliminatedPremiseNodes.empty())
			{
				std::cout << "Eliminated premise instances/nodes: ";
				for (const auto& eliminatedNode : eliminatedPremiseNodes)
				{
					std::cout << "[" << eliminatedNode->GetID() << "]";
				}
				std::cout << std::endl;
			}

			return true;
		}
		else
		{
			error = rule.GetError();
			return false;
		}
	}
}