#include "function_term.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

FunctionTerm::FunctionTerm(const FunctionSymbol &symbol, const std::vector<Term> &terms)
  : BaseTerm (), m_symbol{symbol}, m_terms{terms}
{
}

std::ostream &FunctionTerm::print(std::ostream &out) const
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

bool FunctionTerm::equalTo(const Term &oth) const
{
  const FunctionTerm *pOth = dynamic_cast<const FunctionTerm*>(oth.get());
  if (pOth)
  {
    return symbol() == pOth->symbol() && 
        m_terms.size() == pOth->m_terms.size() &&
        std::equal(m_terms.cbegin(), m_terms.cend(), pOth->m_terms.cbegin());
  }
  else
  {
    return false;
  }
}

void FunctionTerm::getVariables(VariablesSet &vset) const
{
  for (const auto &term : m_terms)
  {
    term->getVariables(vset);
  }
}

bool FunctionTerm::hasVariable(const Variable &v) const
{
  for (const auto &term : m_terms)
  {
    if (term->hasVariable(v))
    {
      return true;
    }
  }
  return false;
}

Term FunctionTerm::substitute(const Variable &v, const Term &t) const
{
  std::vector<Term> terms;
  terms.reserve(m_terms.size());
  std::transform(m_terms.cbegin(), m_terms.cend(), std::back_inserter(terms), [&](const Term &el) { return el->substitute(v, t); });
  return std::make_shared<FunctionTerm>(m_symbol, terms);
}

Term FunctionTerm::substitute(const Substitution &s) const
{
    std::vector<Term> modifiedTerms;
    modifiedTerms.reserve(m_terms.size());
    for (const Term & t : m_terms)
    {
        modifiedTerms.push_back(t->substitute(s));
    }
    return std::make_shared<FunctionTerm>(m_symbol, modifiedTerms);
}
