#ifndef NOT_H
#define NOT_H

#include "unary_connective.h"

class Not : public UnaryConnective
{
public:
    Not(const Formula &f);
    
    virtual Formula substitute(const Variable & v, const Term & t) const;
    
    virtual Formula substitute(const Substitution &s) const;
    
    virtual std::ostream& print(std::ostream & out) const;
};

#endif // NOT_H
