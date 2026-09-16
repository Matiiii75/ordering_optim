#pragma once 

#include "Data.hpp"

struct PreTraitement 
{   
    const Data& data;                                       // data originales

    PreTraitement(const Data& _data): data(_data) {}
    
    // calcule toutes les composantes connexes, et mémorise leurs tailles
    std::vector<std::vector<int>> compute_composantes();
    
    // calcule le sous graphe induit (ré-indexé) par une composante
    Dag compute_induced_dag(const std::vector<int>& composante);  
    
    // calcule les sous-instances induitent par composantes faiblement connexes 
    void compute_sub_instances();
};