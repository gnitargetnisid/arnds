#ifndef UNIFICATION_H
#define UNIFICATION_H

#include "common.h"
#include "base_term.h"
#include "base_formula.h"

#include <vector>
#include <experimental/optional>
#include <iostream>

/**
 * Niz parova termova koje cemo koristiti u supstituciji
 */
using TermPairs = std::vector<std::pair<Term, Term>>;

/**
 * Ako je skup termova unifikabilan vracamo supstituciju inace nista,
 * zbog toga nam treba opciona substitucija
 */
using OptionalSubstitution = std::experimental::optional<Substitution>;

/**
 * @brief unify - unifikuje skup parova termova ako je to moguce
 * @param termPairs - parovi termova koje treba unifikovati
 * @return najopstiji unifikator
 */
OptionalSubstitution unify(const TermPairs &termPairs);

/**
 * @brief unify - unifikuje skup parova termova ako je to moguce
 * @param termPairs - parovi termova koje treba unifikovati
 * @param s - referenca na supstituciju koju popunjavamo ako su unifikabilni
 * @return true ako su parovi unifikabilni, false inace
 */
bool unify(const TermPairs &termPairs, Substitution &s);

/**
 * @brief operator << - ispisuje supstituciju u citljivom formatu
 * @param out - stream u koji se vrsi ispis
 * @param s - supstitucija koja se ispisuje
 * @return referencu na izmenjeni stream
 */
std::ostream& operator<<(std::ostream &out, const Substitution &s);

/**
 * @brief operator << - ispisuje parove termova u citljivom formatu
 * @param out - stream u koji se vrsi ispis
 * @param pairs - parovi koji se ispisuju
 * @return referencu na izmenjeni stream
 */
std::ostream& operator<<(std::ostream &out, const TermPairs &pairs);

#endif // UNIFICATION_H
