#include "signature.h"

unsigned Signature::s_UniqueCounter = 0U;

void Signature::addFunctionSymbol(const FunctionSymbol &fsym, Arity ar)
{
  m_functions[fsym] = ar;
}

void Signature::addPredicateSymbol(const RelationSymbol &fsym, Arity ar)
{
  m_predicates[fsym] = ar;
}

bool Signature::hasFunctionSymbol(const FunctionSymbol &fsym, const Arity &ar) const 
{
  auto it = m_functions.find(fsym);
  if (it == m_functions.cend())
  {
    return false;
  }
  else
  {
    return it->second == ar;
  }
}

bool Signature::hasPredicateSymbol(const RelationSymbol &psym, const Arity &ar) const
{
  auto it = m_predicates.find(psym);
  if (it == m_predicates.cend())
  {
    return false;
  }
  else
  {
    return it->second == ar;
  }
}

FunctionSymbol Signature::getUniqueFunctionSymbol() const
{
    FunctionSymbol name;
    do {
        name = "uf" + std::to_string(s_UniqueCounter++);
    } while (m_functions.cend() != m_functions.find(name));
    return name;
}
