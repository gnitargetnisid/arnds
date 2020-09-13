#ifndef BASEFORMULA_H
#define BASEFORMULA_H

#include "common.h"
#include "base_term.h"

#include <iostream>
#include <memory>

class BaseFormula;

using Formula = std::shared_ptr<BaseFormula>;

class BaseFormula : public std::enable_shared_from_this<BaseFormula>
{
public:
    BaseFormula();
    
    virtual ~BaseFormula();
    
    virtual std::ostream& print(std::ostream & out) const = 0;
    
    virtual unsigned complexity() const = 0;
  
    virtual bool equalTo(const Formula & f) const;
  
    virtual void getVars(VariablesSet & vars, bool free = false) const = 0;
  
    bool hasVariable(const Variable & v, bool free = false) const;
  
    virtual Formula substitute(const Variable & v, const Term & t) const = 0;
    
    virtual Formula substitute(const Substitution &s) const;

	std::string getText();
    
    template <typename Derived>
    static inline const Derived* isOfType(const Formula &f)
    {
        return dynamic_cast<const Derived*>(f.get());
    }

	template <typename T>
	bool IsA() { return dynamic_cast<T*>(this) != nullptr; }

	template <typename T>
	T* As() { return static_cast<T*>(this); }
    
};

std::ostream& operator<<(std::ostream &out, const Formula &f);

#endif // BASEFORMULA_H
