#pragma once 

#include "Data.hpp"
#include "Criteres.hpp"
#include "Common.hpp"
#include "State_graph.hpp"
#include "Timer.hpp"

struct FPT_solver 
{
    const Data& data; 
    State_graph& SG; 
    const Criteres& critere; 

    std::queue<int> L; 
    std::vector<int> best_dist; 
    std::vector<std::pair<int,int>> pred_in_pcc; 

    double time_limit;  
    Timer timer; 

    bool found_solution; 
    double total_time;
    int optimal_value; 

    FPT_solver(
        const Data& _data, 
        State_graph& _SG, 
        const Criteres& _critere, 
        double time_limit
    ); 

    std::vector<int> rebuild_opt_order() const; 

    /**
     * @brief Calcul le cutset associé à un ensemble Candidat 
     * @param cand ens. cand. considéré 
     * @param cut_set_size passé par référence, pour récup la taille du cutset calculé
     * @param cut_set passé par référence, pour récupéré le cutset calculé
     * @param hors_cut_set passé par référence, pour récupéré V-S
     */
    void compute_cut_set(
        const std::vector<int>& cand, 
        int& cut_set_size, 
        std::vector<uint8_t>& cut_set, 
        std::vector<int>& hors_cut_set
    ) const; 

    /**
     * @brief Vérifie si la limite de temps est dépassée 
     * @param iteration_count iteration compteur (on évalue que si c'est un multiple de 10000)
     * @return true si on a pas atteint la limite, false sinon 
     */
    bool try_eval_time_limit(int iteration_count); 
    
    /**
     * @brief Met à jour le label de distance d'un ensemble candidat
     */
    inline void update_distance_label(int C1_ID, int C2_ID, int curr_c) 
    {
        int best_dist_C1 = best_dist[C1_ID]; 
        int weight_C1 = SG.weights[C1_ID]; 
        int dist_from_C1 = critere.compute_dist_from_pred(best_dist_C1, weight_C1); 
        if(best_dist[C2_ID] > dist_from_C1) {
            best_dist[C2_ID] = dist_from_C1;
            pred_in_pcc[C2_ID] = {C1_ID, curr_c}; 
        }
    } 

    /**
     * @brief Solveur pour l'algorithme FPT. Il construit le graphe d'états. 
     */
    void solve(); 

    SolverResults get_results() const; 
}; 