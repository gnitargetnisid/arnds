#include "or.h"
#include "constants.h"
#include "forall.h"
#include "exists.h"

Or::Or(const Formula &op1, const Formula &op2)
    : BinaryConnective (op1, op2)
{
    
}

std::ostream &Or::print(std::ostream &out) const
{
    return printImpl(out, "\\/");
}

Formula Or::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<Or>(v, t);
}

Formula Or::substitute(const Substitution &s) const
{
    return std::make_shared<Or>(m_op1->substitute(s), m_op2->substitute(s));
}