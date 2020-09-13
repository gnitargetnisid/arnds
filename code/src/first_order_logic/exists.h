#ifndef EXISTS_H
#define EXISTS_H

#include "quantifier.h"

class Exists : public Quantifier
{
public:
    Exists(const Variable &var, const Formula &f);
    
    virtual Formula substitute(const Variable & v, const Term & t) const;
    
    virtual Formula substitute(const Substitution &s) const;
    
    virtual std::ostream& print(std::ostream & out) const;
};

#endif // EXISTS_H
