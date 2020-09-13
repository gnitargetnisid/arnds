#include "unification.h"
#include "variable_term.h"
#include "function_term.h"

#include <algorithm>

static void factoring(TermPairs &termPairs)
{
    /* Za svaki par termova proveravamo da li postoji par koji mu je jednak */
    for (size_t i = 0; i < termPairs.size(); ++i)
    {
        for (size_t j = i+1; j < termPairs.size(); )
        {
            /* Ako su parovi jednaki izbacujemo par indeksiran sa j */
            if (termPairs[i].first->equalTo(termPairs[j].first) && 
                    termPairs[i].second->equalTo(termPairs[j].second))
            {
                /* Zamena sa krajnjim elementom i izbacivanje 
         * poslednjeg elementa nakon zamene */
                std::swap(termPairs[j], termPairs.back());
                termPairs.pop_back();
            }
            else /* Ukoliko parovi nisu isti prelazimo na sledeci par */
            {
                ++j;
            }
        }
    }
}

static void tautology(TermPairs &termPairs)
{
    /* Prolazimo kroz sve parove i eliminisemo one koji sadrze 2 ista terma */
    for (size_t i = 0; i < termPairs.size(); )
    {
        if (termPairs[i].first->equalTo(termPairs[i].second))
        {
            std::swap(termPairs[i], termPairs.back());
            termPairs.pop_back();
        }
        else
        {
            ++i;
        }
    }
}

static bool orientation(TermPairs &termPairs)
{
    /* Za svaki par termova proveravamo da li je drugi promenljiva a prvi ne,
   * ako to jeste slucaj menjamo im mesta
   */
    bool change = false;
    for (auto &termPair : termPairs)
    {
        if (dynamic_cast<VariableTerm*>(termPair.second.get()) && 
                !dynamic_cast<VariableTerm*>(termPair.first.get()))
        {
            std::swap(termPair.first, termPair.second);
            change = true;
        }
    }
    return change;
}

static bool decomposition(TermPairs &termPairs, bool &collision)
{
    /* Za sve parove */
    bool change = false;
    for (size_t i = 0; i < termPairs.size();)
    {
        /* Proveravamo da li su oba clana para funkcijski termovi */
        FunctionTerm *first = dynamic_cast<FunctionTerm*>(termPairs[i].first.get());
        FunctionTerm *second = dynamic_cast<FunctionTerm*>(termPairs[i].second.get());
        if (first && second)
        {
            /* Ako im se simboli razlikuju unifikacija nije uspela */
            if (first->symbol() != second->symbol())
            {
                collision = true;
                return false;
            }
            else
            {
                /* Simboli su im isti, dodajemo u listu parova parove operanada */
                for (size_t j =0; j < first->operands().size(); ++j)
                {
                    termPairs.emplace_back(first->operands()[j], second->operands()[j]);
                }
                
                /* Brisemo tekuci par nakon dekompozicije */
                std::swap(termPairs[i], termPairs.back());
                termPairs.pop_back();
                change = true;
            }
        }
        else
        {
            ++i;
        }
    }
    
    return change;
}

static bool application(TermPairs &termPairs, bool &cycle)
{
    /* Primenjujemo supstituciju v->t za sve parove oblika <v, t>, 
     * originalni par se eliminise a supstitucija se primenjuje na
     * sve ostale parove
     */
    bool change = false;
    for (size_t i = 0; i < termPairs.size(); ++i)
    {
        /* Proveravamo da li je prvi clan para promenljiva */
        VariableTerm *first = dynamic_cast<VariableTerm*>(termPairs[i].first.get());
        Term second = termPairs[i].second;
        if (first)
        {
            /* Ako drugi clan para tj. term sadrzi promenljivu koja je prvi clan
             * unifikacija nije uspela
             */
            if (second->hasVariable(first->variable()))
            {
                cycle = true;
                return false;
            }
            else
            {
                /* Za sve parove termova osim tekuceg para primeni supstituciju */
                for (size_t j = 0; j < termPairs.size(); ++j)
                {
                    auto &tpJ = termPairs[j];
                    if (i != j)
                    {
                        /* Supstituciju primenjujemo na oba clana para termova */
                        if (tpJ.first->hasVariable(first->variable()))
                        {
                            tpJ.first = tpJ.first->substitute(first->variable(), second);
                            change = true;
                        }
                        if (tpJ.second->hasVariable(first->variable()))
                        {
                            tpJ.second = tpJ.second->substitute(first->variable(), second);
                            change = true;
                        }
                    }
                }
            }
        }
    }
    
    return change;
}

static bool unify(TermPairs &termPairs)
{
    bool repeat = false;
    bool cycle = false;
    bool collision = false;
    do {
        
        factoring(termPairs);
        tautology(termPairs);
        repeat = orientation(termPairs) || 
                decomposition(termPairs, collision) || 
                application(termPairs, cycle);
        
        if (collision || cycle)
        {
            return false;
        }
    } while (repeat);
    
    return true;
} 

OptionalSubstitution unify(const TermPairs &termPairs)
{
    /* Kopiramo skup termova da bismo mogli da ga menjamo i unifikujemo  */
    TermPairs cpyPairs = termPairs;
    if (!unify(cpyPairs))
    {
        return {};
    }
    
    /* Iz skupa parova izvlacimo supstituciju */
    Substitution s;
    for (const auto &termsPair : cpyPairs)
    {
        const VariableTerm *vterm = static_cast<const VariableTerm*>(termsPair.first.get());
        s[vterm->variable()] = termsPair.second;
    }
    
    return s;
}

std::ostream &operator<<(std::ostream &out, const Substitution &s)
{
    out << "[\t";
    for (const auto &varTermPair : s)
    {
        out << " " << varTermPair.first << "->" << varTermPair.second << "\t";
    }
    return out << "]";
}

std::ostream &operator<<(std::ostream &out, const TermPairs &pairs)
{
    out << "{ ";
    for (const auto &termPair : pairs)
    {
        out << "(" << termPair.first << "," << termPair.second << ") ";
    }
    return out << "}";
}

bool unify(const TermPairs &termPairs, Substitution &s)
{
    TermPairs cpyPairs = termPairs;
    if (!unify(cpyPairs))
    {
        return false;
    }
    
    /* Iz skupa parova izvlacimo supstituciju */
    s.clear();
    for (const auto &termsPair : cpyPairs)
    {
        const VariableTerm *vterm = static_cast<const VariableTerm*>(termsPair.first.get());
        s[vterm->variable()] = termsPair.second;
    }
    
    return true;
}
