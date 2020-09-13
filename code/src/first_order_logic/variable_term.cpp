#include "variable_term.h"

VariableTerm::VariableTerm(const Variable &var)
    : m_var{var}
{
    
}

std::ostream &VariableTerm::print(std::ostream &out) const
{
    return out << m_var;
}

bool VariableTerm::equalTo(const Term &oth) const
{
    const VariableTerm *pOth = dynamic_cast<const VariableTerm*>(oth.get());
    if (pOth)
    {
        return pOth->variable() == variable();
    }
    else
    {
        return false;
    }
}

void VariableTerm::getVariables(VariablesSet &vset) const
{
    vset.insert(variable());
}

bool VariableTerm::hasVariable(const Variable &v) const
{
    return v == variable();
}

Term VariableTerm::substitute(const Variable &v, const Term &t) const
{
    if (v == m_var)
    {
        return t;
    }
    else
    {
        return std::const_pointer_cast<BaseTerm>(shared_from_this());
    }
}

Term VariableTerm::substitute(const Substitution &s) const
{
    for (const auto & varTermPair : s)
    {
        if (m_var == varTermPair.first)
        {
            return varTermPair.second;
        }
    }
    return std::const_pointer_cast<BaseTerm>(shared_from_this());
}
