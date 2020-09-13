#ifndef BINARYCONNECTIVE_H
#define BINARYCONNECTIVE_H

#include "base_formula.h"

#define GET_OPERANDS_EXT(f, op1, op2) \
    Formula op1, op2; \
    std::tie(op1, op2) = f->operands()

class BinaryConnective : public BaseFormula
{
public:
    BinaryConnective(const Formula &op1, const Formula &op2);
    
    virtual unsigned complexity() const;
  
    virtual bool equalTo(const Formula & f) const;
  
    virtual void getVars(VariablesSet & vars, bool free = false) const;
  
    inline std::pair<Formula, Formula> operands() const { return {m_op1, m_op2}; }

protected:
    template <typename Derived>
    Formula substituteImpl(const Variable & v, const Term & t) const;
    
    std::ostream& printImpl(std::ostream &out, const std::string &symbol) const;
    
protected:
    Formula m_op1;
    Formula m_op2;
};

template <typename Derived>
Formula BinaryConnective::substituteImpl(const Variable &v, const Term &t) const
{
  return std::make_shared<Derived>(m_op1->substitute(v, t), m_op2->substitute(v, t));
}

#endif // BINARYCONNECTIVE_H
