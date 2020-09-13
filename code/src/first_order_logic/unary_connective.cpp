#include "unary_connective.h"

UnaryConnective::UnaryConnective(const Formula &f)
    : BaseFormula (), m_op(f)
{
  
}

unsigned UnaryConnective::complexity() const
{
  return m_op->complexity() + 1;
}

bool UnaryConnective::equalTo(const Formula &f) const
{
  if (BaseFormula::equalTo(f))
  {
    return m_op->equalTo(static_cast<const UnaryConnective*>(f.get())->m_op);
  }
  return false;
}

void UnaryConnective::getVars(VariablesSet &vars, bool free) const
{
  m_op->getVars(vars, free);
}
