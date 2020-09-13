#include "binary_connective.h"

#include <stdexcept>

BinaryConnective::BinaryConnective(const Formula &op1, const Formula &op2)
  : BaseFormula (), m_op1(op1), m_op2(op2)
{
}

unsigned BinaryConnective::complexity() const
{
  return 1 + m_op1->complexity() + m_op2->complexity();
}

bool BinaryConnective::equalTo(const Formula &f) const
{
  if (BaseFormula::equalTo(f))
  {
    return m_op1->equalTo(static_cast<const BinaryConnective*>(f.get())->m_op1) &&
        m_op2->equalTo(static_cast<const BinaryConnective*>(f.get())->m_op2);
  }
  return false;
}

void BinaryConnective::getVars(VariablesSet &vars, bool free) const
{
  m_op1->getVars(vars, free);
  m_op2->getVars(vars, free);
}

std::ostream &BinaryConnective::printImpl(std::ostream &out, const std::string &symbol) const
{
  out << '(';
  m_op1->print(out);
  out << ' ' << symbol << ' ';
  m_op2->print(out);
  return out << ')';
}
