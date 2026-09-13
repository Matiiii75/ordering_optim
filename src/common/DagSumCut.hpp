#pragma once 

#include "Criteres.hpp"

struct DagSumCut : Criteres 
{    
    const Data& data; 

    DagSumCut(const Data& _data): data(_data) {} 

    virtual int compute_weight(
        int pred_cand_set_weight, 
        int cand_c, 
        const std::vector<uint8_t>& cut_set
    ) const override;  // override indique au compilateur que cette méthode redéfinit une méthode virtual

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
}; 


