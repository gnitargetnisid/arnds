#include "and.h"
#include "constants.h"
#include "forall.h"
#include "exists.h"

And::And(const Formula &op1, const Formula &op2)
    : BinaryConnective (op1, op2)
{
    
}

std::ostream &And::print(std::ostream &out) const
{
    return printImpl(out, "/\\");
}

Formula And::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<And>(v, t);
}

Formula And::substitute(const Substitution &s) const
{
    return std::make_shared<And>(m_op1->substitute(s), m_op2->substitute(s));
}
