#include "atomic_formula.h"

AtomicFormula::AtomicFormula()
  : BaseFormula ()
{
  
}

unsigned AtomicFormula::complexity() const
{
    return 0;  
}

void AtomicFormula::getVars(VariablesSet &vars, bool free) const
{
    UNUSED_ARG(vars);
    UNUSED_ARG(free);
}

Formula AtomicFormula::substitute(const Variable &v, const Term &t) const
{
    UNUSED_ARG(v);
    UNUSED_ARG(t);
    return std::const_pointer_cast<BaseFormula>(shared_from_this());
}
