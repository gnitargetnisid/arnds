#ifndef ATOMICFORMULA_H
#define ATOMICFORMULA_H

#include "base_formula.h"
#include "common.h"

class AtomicFormula : public BaseFormula
{
public:
    AtomicFormula();
    
    virtual unsigned complexity() const;
    
    virtual void getVars(VariablesSet & vars, bool free) const;
    
    virtual Formula substitute(const Variable & v, const Term & t) const;
};

#endif // ATOMICFORMULA_H
