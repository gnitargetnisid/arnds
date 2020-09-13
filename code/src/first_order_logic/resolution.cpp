#include "resolution.h"
#include "first_order_logic.h"
#include "unification.h"

#include <algorithm>
#include <iterator>

static OptionalSubstitution unify(const Atom *a1, const Atom *a2)
{
    /**
     * Ovo je pomocna funkcija u kojoj vadimo parove termova iz atoma 
     * kako bismo probali da unifikujemo 2 atoma.
     */
    
    if (a1->symbol() != a2->symbol())
    {
        return {};
    }
    
    TermPairs tpairs;
    const std::vector<Term> &ops1 = a1->operands();
    const std::vector<Term> &ops2 = a2->operands();
    tpairs.reserve(ops1.size());
    for (size_t i = 0; i < ops1.size(); ++i)
    {
        tpairs.emplace_back(ops1[i], ops2[i]);
    }
    return unify(tpairs);
}

static bool clauseExists(const CNF &cnf, const Clause &c)
{
    /**
     * Uslov koji nas zanima je malo labaviji nego da postoji bas takva klauza 'c'
     * u formuli 'cnf'. Posmatrajmo klauze:
     * c1 = (p \/ q \/ r) i c2 = (p \/ q)
     * Ukoliko se c2 nalazi u posmatranoj formuli, nema puno smisla dodavati c1
     * kao novu klauzu. Ako je c2 zadovoljena to znaci da je bar jedan od literala p, q
     * tacan, sto dalje dovodi do toga da je c1 takodje zadovoljena. Klauza c1 nam 
     * ne daje nikakve nove restrikcije i nema smisla da je dodajemo (mozemo reci da
     * je c2 sadrzana u c1).
     */
    
    /* Za sve klauze posmatrane formule proveravamo sadrzanost sa prosledjenom klauzom */
    for (const auto &c1 : cnf)
    {
        /* Ako se svi literali 'c1' nalaze u 'c' smatramo da klauza vec postoji */
        bool subsumed = true;
        for (const auto &l : c1)
        {
            if (std::find(c.cbegin(), c.cend(), l) == c.cend())
            {
                subsumed = false;
                break;
            }
        }
        
        if (subsumed)
        {
            return true;
        }
    }
    return false;
}

static bool clauseTautology(const Clause &c)
{
    /* Klauze ja tautologija ako sadrzi suprotne literale */
    for (const auto &l : c)
    {
        /* Ako je u pitanju atom, trazimo njegovu negaciju, inace operand Not-a */
        Formula opositeL;
        const Atom *a = dynamic_cast<const Atom*>(l.get());
        if (a)
        {
            opositeL = std::make_shared<Not>(l);
        }
        else
        {
            opositeL = static_cast<const Not*>(l.get())->operand();
        }
        
        if (c.cend() != std::find(c.cbegin(), c.cend(), opositeL))
        {
            return true;
        }
    }
    return false;
}

static bool tryGroupLiterals(CNF &cnf, unsigned idx)
{
    /**
     * Trudimo se da unifikujemo sve parove literala klauze, zato
     * definisemo promenljive da je i-ti literal atom 'ai', da je
     * i-ti literal negacija atoma 'ni' i sl.
     */
    bool ret = false;
    Clause &c = cnf[idx];
    Atom *ai = nullptr, *aj = nullptr;
    Not *ni = nullptr, *nj = nullptr;
    
    /**
     * Za sve parove literala ukoliko su istog tipa trudimo se da ih unifikujemo
     * */
    for (size_t i = 0; i < c.size(); ++i)
    {
        /* Ako dinamicko kastovanje u Atom* nije uspelo, znaci da je literal sigurno Not */
        ai = dynamic_cast<Atom*>(c[i].get());
        ni = !ai ? static_cast<Not*>(c[i].get()) : nullptr;
        for (size_t j = i+1; j < c.size(); ++j)
        {
            /* Ako dinamicko kastovanje u Atom* nije uspelo, znaci da je literal sigurno Not */
            aj = dynamic_cast<Atom*>(c[j].get());
            nj = !aj ? static_cast<Not*>(c[j].get()) : nullptr;
            OptionalSubstitution s;
            if (ai && aj)
            {
                s = unify(ai, aj);
            }
            else if (ni && nj)
            {
                s = unify(static_cast<Atom*>(ni->operand().get()), 
                          static_cast<Atom*>(nj->operand().get()));
            }
            
            /** 
             * Ako su unifikabilni izbacujemo jedan od dva literala, a na sve ostale 
             * primenjujemo supstituciju 
             */
            if (s)
            {
                Clause cCpy = cnf[idx];
                std::swap(cCpy[j], cCpy.back());
                cCpy.pop_back();
                for (auto &l : cCpy)
                {
                    l = l->substitute(s.value());
                }
                
                /* Ako klauza vec postoji ili je tautologija nas skup klauza se sustinski ne menja */
                if (!clauseExists(cnf, cCpy) && !clauseTautology(cCpy))
                {
                    cnf.push_back(cCpy);
                    ret = true;
                }
            }
        }
    }
    
    return ret;
}

static bool grouping(CNF &cnf, unsigned &idxLastGrpCl)
{
    /**
     * Na sve klauze pocevsi od poslednje za koju smo to vec radili,
     * trudimo se da primenimo grupisanje
     */
    bool ret = false;
    while (idxLastGrpCl < cnf.size()) 
    {
        if (tryGroupLiterals(cnf, idxLastGrpCl++))
        {
            ret = true;
        }
    }
    return ret;
}

static void getClauseVars(const Clause &c, VariablesSet &vset)
{
    for (const auto & l: c)
    {
        l->getVars(vset);
    }
}

static bool clauseHasVar(const Clause &c, const Variable &v)
{
    VariablesSet vset;
    getClauseVars(c, vset);
    return vset.find(v) != vset.cend();
}

static Variable getUniqueVar(const Clause &c1, const Clause &c2)
{
    static unsigned s_UniqueCounter = 0U;
    VariablesSet vset;
    getClauseVars(c1, vset);
    getClauseVars(c2, vset);
    Variable unique;
    do {
        unique = "uv" + std::to_string(s_UniqueCounter++);
    } while (vset.find(unique) != vset.cend());
    return unique;
}

static bool tryResolveClauses(CNF &cnf, unsigned i, unsigned j)
{
    /* Obezbedjujemo korektno (pre)imenovanje promenljivih */
    bool ret = false;
    VariablesSet vset;
    getClauseVars(cnf[i], vset);
    for (const auto &v : vset)
    {
        if (clauseHasVar(cnf[j], v))
        {
            Variable renamed = getUniqueVar(cnf[i], cnf[j]);
            for (auto &l : cnf[j])
            {
                l = l->substitute(v, std::make_shared<VariableTerm>(renamed));
            }
        }
    }
    
    /* Za sve parove literala klauza probamo da ih unifikujemo ako nisu istog tipa (Atom i Not) */
    for (size_t k = 0; k < cnf[i].size(); ++k)
    {
        /* Ako dinamicko kastovanje u Atom* nije uspelo, znaci da je literal sigurno Not */
        Atom *ai = dynamic_cast<Atom*>(cnf[i][k].get());
        Not *ni = !ai ? static_cast<Not*>(cnf[i][k].get()) : nullptr;
        for (size_t l = 0; l < cnf[j].size(); ++l)
        {
            /* Ako dinamicko kastovanje u Atom* nije uspelo, znaci da je literal sigurno Not */
            Atom *aj = dynamic_cast<Atom*>(cnf[j][l].get());
            Not *nj = !aj ? static_cast<Not*>(cnf[j][l].get()) : nullptr;
            OptionalSubstitution s;
            if (ai && nj)
            {
                /* Unifikaciju vrsimo nad operandom Not-a */
                s = unify(ai, static_cast<Atom*>(nj->operand().get()));
                
            }
            else if (ni && aj)
            {
                /* Unifikaciju vrsimo nad operandom Not-a */
                s = unify(static_cast<Atom*>(ni->operand().get()), aj);
            }
            
            /* Ako je unifikacija uspela */
            if (s)
            {
                /* Izbacujemo suprotne literale iz maticnih klauza */
                Clause cnfCpyI = cnf[i];
                Clause cnfCpyJ = cnf[j];
                std::swap(cnfCpyI[k], cnfCpyI.back());
                cnfCpyI.pop_back();
                std::swap(cnfCpyJ[l], cnfCpyJ.back());
                cnfCpyJ.pop_back();
                
                /* Kopiramo preostale literale iz obe klauze u rezolventu, 
                 * primenjujuci supstituciju usput */
                Clause resolvent;
                resolvent.reserve(cnfCpyI.size() + cnfCpyJ.size());
                std::transform(cnfCpyI.cbegin(), 
                               cnfCpyI.cend(), 
                               std::back_inserter(resolvent),
                               [&](const Formula &l) {
                    return l->substitute(s.value());
                });
                std::transform(cnfCpyJ.cbegin(), 
                               cnfCpyJ.cend(), 
                               std::back_inserter(resolvent),
                               [&](const Formula &l) {
                    return l->substitute(s.value());
                });
                
                /* Ako je rezolventa tautologija ili smo vec izveli takvu klauzu ignorisemo je */
                if (!clauseTautology(resolvent) && !clauseExists(cnf, resolvent))
                {
                    cnf.push_back(resolvent);
                    ret = true;
                }
            }
        }
    }
    
    return ret;
}

static bool resolventFound(CNF &cnf, unsigned &idxPrev, unsigned &idxCurr)
{
    /* Za sve parove klauza za koje rezolucija nije primenjena do sada, primeni je*/
    bool ret = false;
    while (idxCurr < cnf.size())
    {
        if (tryResolveClauses(cnf, idxPrev, idxCurr))
        {
            ret = true;
        }
        
        /* Ako je prethodna klauza nije dosla do tekuce, pomeri indeks prethodne ka kraju */
        if (idxPrev < idxCurr - 1)
        {
            ++idxPrev;
        }
        else /* Inace, pomeri indeks tekuce klauze ka kraju i pocni ispocetka */
        {
            idxPrev = 0;
            ++idxCurr;
        }
    }
    
    return ret;
}

bool resolution(const CNF &cnf)
{
    /**
     * Pravimo kopiju ulazne formule zbog modifikacija koje cemo vrsiti. Uvodimo indekse
     * za poslednju klauzu na koju smo primenili grupisanje 'idxLastGrpCl' kao i promenljive
     * za pracenje na koje smo klauze primenili pravilo rezolucije 'idxPrev' i 'idxCurr'.
     * Ideja je da rezolviramo klauze sa desna na levo. Na primer klauze 1 i 0, zatim 2 i 0,
     * zatim 2 i 1, zatim 3 i 0, zatim 3 i 1, zatim 3 i 2 itd. Razlog za ovo je sto stalno 
     * dodajemo nove klauze (rezolvente ili rezultate grupisanja) i na ovaj nacin izbegavamo
     * pozivanje rezolucije vise puta za iste klauze.
     */
    CNF cpyCnf = cnf;
    unsigned idxLastGrpCl = 0;
    unsigned idxPrev = 0;
    unsigned idxCurr = 1;
    
    /**
     * Dok se skup klauza menja, proveravamo da li smo izveli praznu klauzu
     */
    while (grouping(cpyCnf, idxLastGrpCl) || resolventFound(cpyCnf, idxPrev, idxCurr))
    {
        if (cpyCnf.cend() != std::find_if(cpyCnf.cbegin(), 
                                          cpyCnf.cend(), 
                                          [=](const Clause &c) 
        { return c.empty(); }))
        {
            return false;
        }
    }
    return true;
}

std::ostream &operator<<(std::ostream &out, const CNF &cnf)
{
    out << "[";
    for (const auto &c : cnf)
    {
        out << "[ ";
        std::copy(c.cbegin(), c.cend(), std::ostream_iterator<Formula>(out, " "));
        out << "]";
    }
    return out << "]";
}
