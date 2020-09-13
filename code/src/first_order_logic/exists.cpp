#include "exists.h"
#include "function_term.h"

#include <algorithm>
#include <iterator>

Exists::Exists(const Variable &var, const Formula &f)
    : Quantifier (var, f)
{
}

Formula Exists::substitute(const Variable &v, const Term &t) const
{
    return substituteImpl<Exists>(v, t);
}

Formula Exists::substitute(const Substitution &s) const
{
    return substituteImpl<Exists>(s);
}

std::ostream &Exists::print(std::ostream &out) const
{
    return printImpl(out, "E");
}
