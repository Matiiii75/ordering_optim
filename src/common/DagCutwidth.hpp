#pragma once 

#include "Criteres.hpp"

struct DagCutwidth : Criteres
{
    const Data& data; 

    DagCutwidth(const Data& _data): data(_data) {}

    virtual int compute_weight(
        int pred_cand_set_weight, 
        int cand_c, 
        const std::vector<uint8_t>& cut_set
    ) const override; 

    virtual int compute_dist_from_pred(
        int best_dist_pred, 
        int weight_pred
    ) const override; 

    virtual int compute_objective_value(
        const std::vector<int>& ordre_topo
    ) const override;  

    virtual bool checker(
        const std::vector<int>& ordre_topo, 
        int value_found
    ) const override;  

    virtual std::string get_name() const override;
    
    virtual std::unique_ptr<Criteres> clone(const Data& data) const override;
    
    virtual void aggrege_sub_solution(int& global_value, int sub_optimal_value) const override; 

    virtual void aggrege_trivial_components(int& global_value, int nb_two_nodes_comp) const override; 
}; 



