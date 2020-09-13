#ifndef FORALL_H
#define FORALL_H

#include "quantifier.h"


class Forall : public Quantifier
{
public:
    Forall(const Variable &var, const Formula &f);
    
    virtual std::ostream& print(std::ostream & out) const;
  
    virtual Formula substitute(const Variable & v, const Term & t) const;
    
    virtual Formula substitute(const Substitution &s) const;
};

#endif // FORALL_H
