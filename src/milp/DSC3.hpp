#pragma once 

#include "Milp.hpp"

/*
implémentation d'un modèle PLNE résolvant DagSumCut. 
Ce modèle est basé sur [David Coudert, "A note on Integer Linear Programming formulations for linear ordering problems on graphs"]
Il utilise les variables suivantes : 
- PHI(u) >= 0 -> capture le nombre de positions où participe u \in V
- xui = 1 si phi(u) <= i
*/

struct DSC3 : Milp
{   
    DSC3(const Data& _data, double _time_limit)
        : Milp(_data, _time_limit) {} // il appelle le constructeur de la classe mère
    
    virtual void solve(bool verbose = false) override; 

    virtual SolverResults get_results() override;  
}; 