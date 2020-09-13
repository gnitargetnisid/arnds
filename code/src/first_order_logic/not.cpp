#include "not.h"
#include "constants.h"
#include "and.h"
#include "or.h"
#include "imp.h"
#include "iff.h"
#include "forall.h"
#include "exists.h"

Not::Not(const Formula &f)
    : UnaryConnective (f)
{
}

Formula Not::substitute(const Variable &v, const Term &t) const
{
    return std::make_shared<Not>(m_op->substitute(v, t));
}

Formula Not::substitute(const Substitution &s) const
{
    return std::make_shared<Not>(m_op->substitute(s));
}

std::ostream &Not::print(std::ostream &out) const
{
    out << "~(";
    m_op->print(out);
    return out << ")";
}
