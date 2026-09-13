#include "Common.hpp"

bool is_disjoint(const std::vector<int>& succ_gamma, int t, 
    const std::vector<uint8_t>& cut_set) 
{
    for(int u : succ_gamma) {
        if(u == t) continue; // puit -> ignorer 
        if(cut_set[u] == 0) // si il est pas dans le cut_set
            return true; 
    }
    
    return false; 
}

bool is_included(const std::vector<int>& v1, const std::vector<uint8_t>& v2, int t) {

    int v1_size = (int)v1.size(); 

    if(v1_size > 0 && v1.back() == t) // on ignore le puit 
        --v1_size; 

    for(int i = 0; i < v1_size; ++i) { // pr chq index de v1
        int v_i = v1[i]; 
        if(v2[v_i] == 0) return false; // si il est pas dans v2
    }

    return true; // si on arrive là c'est que tout élément de v1 est dans v2
}



