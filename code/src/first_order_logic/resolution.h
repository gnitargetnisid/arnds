#ifndef RESOLUTION_H
#define RESOLUTION_H

#include "base_formula.h"

#include <vector>
#include <iostream>

/**
 * Definicija tipova za klauze i KNF
 */
using Clause = std::vector<Formula>;
using CNF = std::vector<Clause>;

/**
 * @brief resolution - algoritam rezolucije
 * @details Algoritam rezolucije je implementiran kao binarna rezolucija sa grupisanjem.
 * Iako je ovo potpun sistem, postoje formule za koje se algoritam ne zaustavlja jer logika
 * prvog reda nije odluciva.
 * @param cnf - ulazna formula u KNF-u
 * @return true ako je formula zadovoljiva, false inace
 */
bool resolution(const CNF &cnf);

/**
 * @brief operator << - ispisuje KNF formulu u citljivom formatu
 * @param out - stream u koji se ispisuje
 * @param cnf - formula koja se ispisuje
 * @return referencu na izmenjeni stream
 */
std::ostream& operator<<(std::ostream &out, const CNF &cnf);

#endif // RESOLUTION_H
