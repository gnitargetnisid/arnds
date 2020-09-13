#ifndef VARIABLETERM_H
#define VARIABLETERM_H

#include "base_term.h"

class VariableTerm;

class VariableTerm : public BaseTerm
{
public:
    VariableTerm(const Variable &var = {});
    
    inline const Variable& variable() const { return m_var; }
    
    virtual std::ostream& print(std::ostream &out) const;
    
    virtual bool equalTo(const Term &oth) const;
    
    virtual void getVariables(VariablesSet &vset) const;
    
    virtual bool hasVariable(const Variable &v) const;
    
    virtual Term substitute(const Variable &v, const Term &t) const;

    virtual Term substitute(const Substitution &s) const;
private:
    /**
     * @brief m_var je promenljiva koja odgovara termu, njena vrednost se cita iz valuacije
     */
    Variable m_var;
};

#endif // VARIABLETERM_H
