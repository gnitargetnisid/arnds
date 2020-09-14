#include "atom.h"

#include <algorithm>
#include <stdexcept>

Atom::Atom(const RelationSymbol &symbol, const std::vector<Term> &terms)
    : AtomicFormula (), m_symbol(symbol), m_terms(terms)
{
}

std::ostream& Atom::print(std::ostream &out) const
{
    if (m_terms.empty())
    {
      return out << m_symbol;
    }
    else
    {
      out << m_symbol << "(";
      for (auto first = m_terms.cbegin(), last = m_terms.cend(); first + 1 != last; ++first)
      {
        (*first)->print(out);
        out << ", ";
      }
      m_terms.back()->print(out);
      return out << ")";
    }
}

bool Atom::equalTo(const Formula &f) const
{
    const Atom *pF = dynamic_cast<const Atom*>(f.get());
    if (pF)
    {
        return symbol() == pF->symbol() && 
            m_terms.size() == pF->m_terms.size() &&
            std::equal(m_terms.cbegin(), m_terms.cend(), pF->m_terms.cbegin());
    }
    else
    {
        return false;
    }
}

void Atom::getVars(VariablesSet &vars, bool free) const
{
    UNUSED_ARG(free);
    for (const Term &t : m_terms)
    {
        t->getVariables(vars);
    }
}

Arity Atom::getArity() const
{
	return m_terms.size();
}

Formula Atom::substitute(const Variable &v, const Term &t) const
{
    std::vector<Term> terms;
    terms.reserve(m_terms.size());
    std::transform(m_terms.cbegin(), m_terms.cend(), std::back_inserter(terms), [&](const Term &el) { return el->substitute(v, t); });
    return std::make_shared<Atom>(m_symbol, terms);
}

Formula Atom::substitute(const Substitution &s) const
{
    std::vector<Term> modifiedTerms;
    modifiedTerms.reserve(m_terms.size());
    for (const Term &t : m_terms)
    {
        modifiedTerms.push_back(t->substitute(s));
    }
    return std::make_shared<Atom>(m_symbol, modifiedTerms);
}
