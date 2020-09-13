#include "rules.h"
#include "first_order_logic/exists.h"
#include "first_order_logic/forall.h"

namespace ND
{
	std::optional<Formula> NotIntroduction::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Premise premise = GetPremise<0>();

		if (!node->IsFormulaA<False>())
		{
			SetError("Not elimination expects False as the contradiction node. You've provided something else.");
			return std::nullopt;
		}

		auto premiseInstances = m_tree.GetAlivePremiseAncestors(node->GetID(), premise->GetID());
		if (premiseInstances.empty())
		{
			SetError("There are no alive premise ancestors of the formula you provided. Therefore there's nothing to negate.");
			return std::nullopt;
		}

		for (auto& premiseNode : premiseInstances)
		{
			EliminatePremiseNode(premiseNode);
		}

		return std::make_shared<Not>(premise->GetFormula());
	}

	std::optional<Formula> NotElimination::ApplyInternal()
	{
		SolverTreeNode LHSNode = GetInputNode<0>();
		SolverTreeNode RHSNode = GetInputNode<1>();

		Formula negativeLHSFormula = std::make_shared<Not>(LHSNode->GetFormula());
		if (negativeLHSFormula->equalTo(RHSNode->GetFormula()))
		{
			return std::make_shared<False>();
		}
		else
		{
			SetError("The formulas you've provided are not a negation of each other. It's possible that >>>semantically<<< they are, but syntactically, they're not, and that's what we're checking for here.");
			return std::nullopt;
		}
	}

	std::optional<Formula> ConjunctionIntroduction::ApplyInternal()
	{
		SolverTreeNode LHSNode = GetInputNode<0>();
		SolverTreeNode RHSNode = GetInputNode<1>();

		return std::make_shared<And>(LHSNode->GetFormula(), RHSNode->GetFormula());
	}

	std::optional<Formula> ConjunctionEliminationLHS::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		if (!node->IsFormulaA<And>())
		{
			SetError("Conjunction elimination has to be applied on an And formula. You've provided something else.");
			return std::nullopt;
		}

		return (static_cast<And*>(node->GetFormula().get()))->operands().first;
	}

	std::optional<Formula> ConjunctionEliminationRHS::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		if (!node->IsFormulaA<And>())
		{
			SetError("Conjunction elimination has to be applied on an And formula. You've provided something else.");
			return std::nullopt;
		}

		return (static_cast<And*>(node->GetFormula().get()))->operands().second;
	}

	std::optional<Formula> DisjunctionIntroductionLHS::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formula = GetFormula<0>();

		return std::make_shared<Or>(node->GetFormula(), formula);
	}

	std::optional<Formula> DisjunctionIntroductionRHS::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formula = GetFormula<0>();

		return std::make_shared<Or>(formula, node->GetFormula());
	}

	std::optional<Formula> DisjunctionElimination::ApplyInternal()
	{
		SolverTreeNode disjunctionNode = GetInputNode<0>();
		SolverTreeNode deductionNodeLHS = GetInputNode<1>();
		SolverTreeNode deductionNodeRHS = GetInputNode<2>();
		Premise premiseLHS = GetPremise<0>();
		Premise premiseRHS = GetPremise<1>();

		if (!disjunctionNode->IsFormulaA<Or>())
		{
			SetError("First formula provided in disjunction elimination must be a disjunction of the first premise and the second premise. You've provided: \r\n" + disjunctionNode->GetFormula()->getText());
			return std::nullopt;
		}

		Or* disjunctionFormula = disjunctionNode->GetFormula()->As<Or>();
		Formula disjunctionLHS = disjunctionFormula->operands().first;
		Formula disjunctionRHS = disjunctionFormula->operands().second;
		if (!disjunctionLHS->equalTo(premiseLHS->GetFormula()))
		{
			SetError("The first premise formula you've provided doesn't match the left hand side of the disjunction formula you've provided."
				    "\r\nFirst premise: "  + premiseLHS->GetFormula()->getText()
					+ "\r\nLHS of the disjunction: " + disjunctionLHS->getText());
			return std::nullopt;
		}
		if (!disjunctionRHS->equalTo(premiseRHS->GetFormula()))
		{
			SetError("The second premise formula you've provided doesn't match the right hand side of the disjunction formula you've provided."
				"\r\nSecond premise: " + premiseRHS->GetFormula()->getText()
				+ "\r\nRHS of the disjunction: " + disjunctionRHS->getText());
			return std::nullopt;
		}

		if (!deductionNodeLHS->GetFormula()->equalTo(deductionNodeRHS->GetFormula()))
		{
			SetError("The formulas you've provided are not the same. It's possible that >>>semantically<<< they are, but syntactically, they're not, and that's what we're checking for here. Formulas: " 
				+ deductionNodeLHS->GetFormula()->getText() + "\r\n"
				+ deductionNodeRHS->GetFormula()->getText());
			return std::nullopt;
		}

		auto lhsPremiseInstances = m_tree.GetAlivePremiseAncestors(deductionNodeLHS->GetID(), premiseLHS->GetID());
		auto rhsPremiseInstances = m_tree.GetAlivePremiseAncestors(deductionNodeRHS->GetID(), premiseRHS->GetID());

		if (lhsPremiseInstances.empty() || rhsPremiseInstances.empty())
		{
			SetError("One of the premises you've specified does not exist among the ancestors of the corresponding node you've specified.");
			return std::nullopt;
		}

		for (auto& premiseNode : lhsPremiseInstances)
		{
			EliminatePremiseNode(premiseNode);
		}
		for (auto& premiseNode : rhsPremiseInstances)
		{
			EliminatePremiseNode(premiseNode);
		}

		return deductionNodeLHS->GetFormula();
	}

	std::optional<Formula> ImplicationIntroduction::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Premise premise = GetPremise<0>();

		//Iterate up until the leaves and look for the premises to eliminate
		auto premiseInstances = m_tree.GetAlivePremiseAncestors(node->GetID(), premise->GetID());

		if (premiseInstances.empty())
		{
			SetError("There are no alive premise ancestors of the node you provided. Therefore there's nothing to use as an implier.");
			return std::nullopt;
		}

		for (auto& premiseNode : premiseInstances)
		{
			EliminatePremiseNode(premiseNode);
		}

		return std::make_shared<Imp>(premise->GetFormula(), node->GetFormula());
	}

	std::optional<Formula> ImplicationElimination::ApplyInternal()
	{
		SolverTreeNode implierNode = GetInputNode<0>();
		SolverTreeNode implicationNode = GetInputNode<1>();

		if (!implicationNode->IsFormulaA<Imp>())
		{
			SetError("The second formula you provided is not an implication.");
			return std::nullopt;
		}

		Imp* implicationFormula = implicationNode->GetFormula()->As<Imp>();
		if (!implicationFormula->operands().first->equalTo(implierNode->GetFormula()))
		{
			SetError("The implier in the second formula does not match the first formula.");
			return std::nullopt;
		}

		return implicationFormula->operands().second;
	}

	std::optional<Formula> ContradictionElimination::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formula = GetFormula<0>();

		if (!node->IsFormulaA<False>())
		{
			SetError("The formula you provided is not a contradiction/False.");
			return std::nullopt;
		}

		return formula;
	}

	std::optional<Formula> TrueIntroduction::ApplyInternal()
	{
		return std::make_shared<True>();
	}

	bool CanConvertFormulaToVariable(Formula formula)
	{
		if (!formula->IsA<Atom>())
		{
			return false;
		}

		VariablesSet varset;
		formula->As<Atom>()->getVars(varset);
		if (!varset.empty())
		{
			return false;
		}

		return true;
	}

	std::optional<Formula> ForallIntroduction::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formulaX = GetFormula<0>();
		Formula formulaY = GetFormula<1>();

		if (!CanConvertFormulaToVariable(formulaX))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formulaX->getText());
			return std::nullopt;
		}

		if (!CanConvertFormulaToVariable(formulaY))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formulaY->getText());
			return std::nullopt;
		}

		Atom* atomFormulaX = formulaX->As<Atom>();
		Atom* atomFormulaY = formulaY->As<Atom>();
		Variable varX = atomFormulaX->symbol();
		Variable varY = atomFormulaY->symbol();

		VariablesSet nodeFreeVarset;
		node->GetFormula()->getVars(nodeFreeVarset, true);

		if (nodeFreeVarset.find(varX) == nodeFreeVarset.end())
		{
			SetError("Variable " + atomFormulaX->getText() + " doesn't exist in formula " + node->GetFormula()->getText());
			return std::nullopt;
		}

		if (nodeFreeVarset.find(varY) != nodeFreeVarset.end())
		{
			SetError("Variable " + atomFormulaY->getText() + " already exists in formula " + node->GetFormula()->getText());
			return std::nullopt;
		}

		const auto& allTreeNodes = m_tree.GetNodes();
		for (const auto& treeNode : allTreeNodes)
		{
			if (treeNode.second->IsA<SolverTreePremiseNodeBase>())
			{
				SolverTreePremiseNodeBase* premiseNode = treeNode.second->As<SolverTreePremiseNodeBase>();
				if (premiseNode->GetEliminated())
				{
					continue;
				}

				VariablesSet formulaVars;
				premiseNode->GetFormula()->getVars(formulaVars, true);
				if (formulaVars.find(varX) != formulaVars.end())
				{
					SetError("Premise [" + std::to_string(premiseNode->GetPremise()->GetID()) + "] Instance [" + std::to_string(premiseNode->GetID()) + "] has " + atomFormulaX->getText() + " as a free variable. This is not allowed.");
					return std::nullopt;
				}
			}
		}

		Formula substituteFormula = node->GetFormula()->substitute(varX, std::make_shared<VariableTerm>(varY));
		return std::make_shared<Forall>(varY, substituteFormula);
	}

	std::optional<Formula> ForallElimination::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formula = GetFormula<0>();

		if (!node->IsFormulaA<Forall>())
		{
			SetError("Expected a forall formula. Got: " + node->GetFormula()->getText());
			return std::nullopt;
		}
		
		if (!CanConvertFormulaToVariable(formula))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formula->getText());
			return std::nullopt;
		}

		Forall* forallFormula = node->GetFormula()->As<Forall>();
		Formula baseFormula = forallFormula->operand();
		Atom* atomFormula = formula->As<Atom>();
		Variable var = atomFormula->symbol();
		
		VariablesSet baseFormulaVarset;
		VariablesSet baseFormulaFreeVarset;
		baseFormula->getVars(baseFormulaVarset);
		baseFormula->getVars(baseFormulaFreeVarset, true);
		if (baseFormulaVarset.find(var) != baseFormulaVarset.end() && baseFormulaFreeVarset.find(var) == baseFormulaFreeVarset.end())
		{
			SetError("Variable " + atomFormula->getText() + " is a bound variable in " + node->GetFormula()->getText());
			return std::nullopt;
		}

		return baseFormula->substitute(forallFormula->variable(), std::make_shared<VariableTerm>(var));
	}

	std::optional<Formula> ExistsIntroduction::ApplyInternal()
	{
		SolverTreeNode node = GetInputNode<0>();
		Formula formulaX = GetFormula<0>();
		Formula formulaY = GetFormula<1>();

		if (!CanConvertFormulaToVariable(formulaX))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formulaX->getText());
			return std::nullopt;
		}

		if (!CanConvertFormulaToVariable(formulaY))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formulaY->getText());
			return std::nullopt;
		}

		Atom* atomFormulaX = formulaX->As<Atom>();
		Atom* atomFormulaY = formulaY->As<Atom>();
		Variable varX = atomFormulaX->symbol();
		Variable varY = atomFormulaY->symbol();

		VariablesSet nodeVarset;
		VariablesSet nodeFreeVarset;
		node->GetFormula()->getVars(nodeVarset);
		node->GetFormula()->getVars(nodeFreeVarset, true);

		if (nodeFreeVarset.find(varX) == nodeFreeVarset.end())
		{
			SetError("Variable " + atomFormulaX->getText() + " doesn't exist as a free variable in formula " + node->GetFormula()->getText());
			return std::nullopt;
		}
		
		if (nodeVarset.find(varY) != nodeVarset.end())
		{
			SetError("Variable " + atomFormulaY->getText() + " exists in " + node->GetFormula()->getText());
			return std::nullopt;
		}

		Formula substituteFormula = node->GetFormula()->substitute(varX, std::make_shared<VariableTerm>(varY));
		return std::make_shared<Exists>(varY, substituteFormula);
	}

	std::optional<Formula> ExistsElimination::ApplyInternal()
	{
		SolverTreeNode existsNode = GetInputNode<0>();
		SolverTreeNode conclusionNode = GetInputNode<1>();
		Premise premise = GetPremise<0>();
		Formula formula = GetFormula<0>();

		if (!existsNode->IsFormulaA<Exists>())
		{
			SetError("Provided formula is not an exists formula. Formula: " + existsNode->GetFormula()->getText());
			return std::nullopt;
		}

		if (!CanConvertFormulaToVariable(formula))
		{
			SetError("Provided variable is in an incorrect format. Variable: " + formula->getText());
			return std::nullopt;
		}

		Atom* atomFormula = formula->As<Atom>();
		Variable var = atomFormula->symbol();

		Exists* existsFormula = existsNode->GetFormula()->As<Exists>();
		Formula baseFormula = existsFormula->operand();
		if (!baseFormula->substitute(existsFormula->variable(), std::make_shared<VariableTerm>(var))->equalTo(premise->GetFormula()))
		{
			SetError("After removing the exists quantifier from " + existsNode->GetFormula()->getText() + " , we don't get " + premise->GetFormula()->getText());
			return std::nullopt;
		}

		VariablesSet conclusionNodeFreeVarset;
		conclusionNode->GetFormula()->getVars(conclusionNodeFreeVarset, true);
		if (conclusionNodeFreeVarset.find(var) != conclusionNodeFreeVarset.end())
		{
			SetError("Found " + atomFormula->getText() + " as a free variable in " + conclusionNode->GetFormula()->getText());
			return std::nullopt;
		}

		const auto& allTreeNodes = m_tree.GetNodes();
		const auto& premiseInstances = premise->GetInstances();
		std::vector<SolverTreePremiseNode> foundPremiseInstances;
		for (const auto& treeNode : allTreeNodes)
		{
			if (treeNode.second->IsA<SolverTreePremiseNodeBase>())
			{
				SolverTreePremiseNode premiseNode = std::dynamic_pointer_cast<SolverTreePremiseNodeBase>(treeNode.second);

				if (premiseNode->GetEliminated())
				{
					continue;
				}

				bool foundInstance = false;
				for (const auto& premiseInstance : premiseInstances)
				{
					if (premiseInstance.get() == premiseNode.get())
					{
						foundPremiseInstances.push_back(premiseNode);
						foundInstance = true;
						break;
					}
				}

				if (foundInstance)
				{
					continue;
				}

				VariablesSet formulaVars;
				premiseNode->GetFormula()->getVars(formulaVars);
				if (formulaVars.find(var) != formulaVars.end())
				{
					SetError("Premise [" + std::to_string(premiseNode->GetPremise()->GetID()) + "] Instance [" + std::to_string(premiseNode->GetID()) + "] has " + atomFormula->getText() + " as a free variable. This is not allowed.");
					return std::nullopt;
				}
			}
		}

		if (foundPremiseInstances.empty())
		{
			SetError("There are no instances of premise " + premise->GetFormula()->getText() + " in the tree of " + conclusionNode->GetFormula()->getText());
			return std::nullopt;
		}

		for (const auto& premiseNode : foundPremiseInstances)
		{
			EliminatePremiseNode(premiseNode);
		}

		return conclusionNode->GetFormula();
	}
}