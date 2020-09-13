#ifndef UNARYCONNECTIVE_H
#define UNARYCONNECTIVE_H

#include "base_formula.h"

class UnaryConnective : public BaseFormula
{
public:
    UnaryConnective(const Formula &f);
    
    Formula operand() const { return m_op; }
    
    virtual unsigned complexity() const;
  
    virtual bool equalTo(const Formula & f) const;
  
    virtual void getVars(VariablesSet & vars, bool free = false) const;

protected:
    Formula m_op;
};

#endif // UNARYCONNECTIVE_H
