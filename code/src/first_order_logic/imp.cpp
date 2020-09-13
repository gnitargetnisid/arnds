#include "imp.h"
#include "constants.h"
#include "not.h"
#include "or.h"

Imp::Imp(const Formula &op1, const Formula &op2)
    : BinaryConnective (op1, op2)
{
    
}

std::ostream &Imp::print(std::ostream &out) const
{
    return printImpl(out, "=>");
}

Formula Imp::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<Imp>(v, t);
}

Formula Imp::substitute(const Substitution &s) const
{
    return std::make_shared<Imp>(m_op1->substitute(s), m_op2->substitute(s));
}