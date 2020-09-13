#include "first_order_logic.h"

#include <iostream>

using namespace std;

int main()
{
    /* Definisemo strukturu */
    Signature::Sptr s = std::make_shared<Signature>();
    
    s->addFunctionSymbol("c", 0);
    s->addFunctionSymbol("g", 1);
    s->addFunctionSymbol("f", 3);
    s->addFunctionSymbol("h", 1);
    s->addPredicateSymbol("p", 2);
    s->addPredicateSymbol("q", 2);
    
    
    
    Term xt = make_shared<VariableTerm>("x");
    Term ut = make_shared<VariableTerm>("u");
    Term vt = make_shared<VariableTerm>("v");
    Term zt = make_shared<VariableTerm>("z");
    Term ct = make_shared<FunctionTerm>(s, "c");
    
    /* g(x) */
    Term gt = make_shared<FunctionTerm>(s, "g", vector<Term> { xt });
    
    /* p(x, g(x)) */
    Formula px = make_shared<Atom>(s, "p", vector<Term> {xt, gt});
    
    /* p(u, v) */
    Formula puv = make_shared<Atom>(s, "p", vector<Term> {ut, vt});
    
    /* q(u, v) */
    Formula quv = make_shared<Atom>(s, "q", vector<Term> {ut, vt});
    
    /* q(c, z) */
    Formula qcz = make_shared<Atom>(s, "q", vector<Term> {ct, zt});
    
    /* h(x) */
    Term hx = make_shared<FunctionTerm>(s, "h", vector<Term> { xt });
    
    Term yt = make_shared<VariableTerm>("y");
    Term wt = make_shared<VariableTerm>("w");
    
    /* g(z) */
    Term gz = make_shared<FunctionTerm>(s, "g", vector<Term> { zt });
    
    /* g(y) */
    Term gy = make_shared<FunctionTerm>(s, "g", vector<Term> { yt });
    
    /* f(x, h(x), y) */
    Term f1 = make_shared<FunctionTerm>(s, "f", vector<Term> { xt, hx, yt });
    
    /* f(g(z), w, z) */
    Term f2 = make_shared<FunctionTerm>(s, "f", vector<Term> { gz, wt, zt });
    
    /* Parovi termova: (g(y), x) i (f(x, h(x), y), f(g(z), w, z)) */
    TermPairs pairs { { gy, xt }, { f1, f2 } };
    
    Substitution sub;
    
    /* Ako su unifikabilni, prikazujemo najopstiji unifikator,
         kao i rezultat primene tog unifikatora na termove */
    if(unify(pairs, sub))
    {
        cout << "Unifiable: " << sub << endl;
        cout << "Check: " << gy->substitute(sub) << " = " << xt->substitute(sub) << " , " << f1->substitute(sub) << " = " << f2->substitute(sub) << endl;
    }
    /* Ako nisu unifikabilni, prikazujemo poruku o tome */
    else
    {
        cout << "Not unifiable" << endl;
    }
    
    /* g(x0) */
    Term x0 = make_shared<VariableTerm>("x0");
    Term g1 = make_shared<FunctionTerm>(s, "g", vector<Term> { x0 });
    
    /* g(x1) */
    Term x1 = make_shared<VariableTerm>("x1");
    Term g2 = make_shared<FunctionTerm>(s, "g", vector<Term> { x1 });
    
    /* g(x2) */
    Term x2 = make_shared<VariableTerm>("x2");
    Term g3 = make_shared<FunctionTerm>(s, "g", vector<Term> { x2 });
    
    Term x3 = make_shared<VariableTerm>("x3");
    
    /* Parovi (x1, g(x0)), (x2, g(x1)), (x3, g(x2)) */
    TermPairs tp { { x1, g1 }, { x2, g2 }, { x3, g3 } };
    
    sub.clear();
    if(unify(tp, sub))
    {
        cout << "Unifiable" << endl;
        cout << sub << endl;
    }
    else
    {
        cout << "Not unifiable" << endl;
    }
    
    
    /* (g(h(x)), g(g(c))) */
    Term ghx = make_shared<FunctionTerm>(s, "g", vector<Term> { make_shared<FunctionTerm>(s, "h", vector<Term> { xt }) });
    Term ggc = make_shared<FunctionTerm>(s, "g", vector<Term> { make_shared<FunctionTerm>(s, "g", vector<Term> { ct }) });
    TermPairs tp2 { { ghx, ggc } };
    sub.clear();
    if(unify(tp2, sub))
    {
        cout << "Unifiable" << endl;
        cout << sub << endl;
    }
    else
    {
        cout << "Not unifiable" << endl;
    }
    
//    CNF cnf(3);
    
//    cnf[0].push_back(px);
//    cnf[1].push_back(make_shared<Not>(puv));
//    cnf[1].push_back(quv);
//    cnf[2].push_back(make_shared<Not>(qcz));
    
    CNF cnf(2);
    
    Formula pcx = make_shared<Atom>(s, "p", vector<Term> { ct, xt });
    Formula pxx = make_shared<Atom>(s, "p", vector<Term> { xt, xt });
    Formula pcy = make_shared<Atom>(s, "p", vector<Term> { ct, yt });
    Formula pyy = make_shared<Atom>(s, "p", vector<Term> { yt, yt });
    
    cnf[0].push_back(make_shared<Not>(pcx));
    cnf[0].push_back(make_shared<Not>(pxx));
    cnf[1].push_back(pcy);
    cnf[1].push_back(pyy);
    
    cout << cnf << endl;
    
    if(resolution(cnf))
    {
        cout << "CNF satisfiable!" << endl;
    }
    else
    {
        cout << "CNF unsatisfiable!" << endl;
    }
    
    return 0;
}
