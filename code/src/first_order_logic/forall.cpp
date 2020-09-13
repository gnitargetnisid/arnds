#include "forall.h"

Forall::Forall(const Variable &var, const Formula &f)
    : Quantifier (var, f)
{
}

std::ostream &Forall::print(std::ostream &out) const
{
    return printImpl(out, "V");
}

Formula Forall::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<Forall>(v, t);
}

Formula Forall::substitute(const Substitution &s) const
{
    return substituteImpl<Forall>(s);
}
