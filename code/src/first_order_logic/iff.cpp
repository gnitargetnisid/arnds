#include "iff.h"
#include "constants.h"
#include "not.h"
#include "and.h"
#include "or.h"

Iff::Iff(const Formula &op1, const Formula &op2)
    : BinaryConnective (op1, op2)
{
}

std::ostream &Iff::print(std::ostream &out) const
{
    return printImpl(out, "<=>");
}

Formula Iff::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<Iff>(v, t);
}

Formula Iff::substitute(const Substitution &s) const
{
    return std::make_shared<Iff>(m_op1->substitute(s), m_op2->substitute(s));
}
