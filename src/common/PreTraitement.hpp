#pragma once 

#include "Data.hpp"
#include "Common.hpp"

struct PreTraitement 
{   
    const Data& data;                                       // data originales
    std::vector<int> one_node_comp; 
    std::vector<std::pair<int,int>> two_nodes_comp;
    std::vector<std::vector<int>> all_map_new_to_old; 
    std::vector<Data> sub_instances; 

    PreTraitement(const Data& _data): data(_data) {}
    
    // calcule toutes les composantes connexes, et mémorise leurs tailles
    std::vector<std::vector<int>> compute_composantes();
    
    // calcule le sous graphe induit (ré-indexé) par une composante
    Dag compute_induced_dag(const std::vector<int>& composante);  
    
    // calcule les sous-instances induites par composantes faiblement connexes 
    void compute_sub_instances();

    std::vector<int> re_label_sub_solution(
        const std::vector<int>& sub_sol,
        int instance_idx
    ) const; 

    SolverResults store_results(
        int optimal_value, 
        int total_cands,
        double total_time,
        const std::vector<int>& optimal_order
    ) const; 

    int count_composantes() const; 
};