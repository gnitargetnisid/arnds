#include "base_term.h"

BaseTerm::BaseTerm()
{
}

BaseTerm::~BaseTerm()
{   
}

bool operator==(const Term &lhs, const Term &rhs)
{
    return lhs->equalTo(rhs);
}

std::ostream &operator<<(std::ostream &out, const Term &t)
{
    return t->print(out);
}
