#include "base_formula.h"

#include <sstream>
#include <typeinfo>

BaseFormula::BaseFormula()
{}

BaseFormula::~BaseFormula()
{}

bool BaseFormula::equalTo(const Formula &f) const
{
    const BaseFormula *base = f.get();
    return typeid (*this) == typeid (*base);
}

bool BaseFormula::hasVariable(const Variable &v, bool free) const
{
  VariablesSet vset;
  getVars(vset, free);
  return vset.find(v) != vset.cend();
}

Formula BaseFormula::substitute(const Substitution &s) const
{
    UNUSED_ARG(s);
    return std::const_pointer_cast<BaseFormula>(shared_from_this());
}


std::string BaseFormula::getText()
{
	std::stringstream str;
	str << ">>> ";
	print(str);
	str << " <<<";
	return str.str();
}

std::ostream& operator<<(std::ostream &out, const Formula &f)
{
    return f->print(out);
}
