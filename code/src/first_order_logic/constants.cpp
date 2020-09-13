#include "constants.h"


True::True()
  : AtomicFormula ()
{
  
}

std::ostream &True::print(std::ostream &out) const
{
    return out << "T";
}

False::False()
  : AtomicFormula ()
{
  
}

std::ostream &False::print(std::ostream &out) const
{
    return out << "F";
}
