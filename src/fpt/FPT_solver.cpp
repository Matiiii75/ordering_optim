#include "FPT_solver.hpp"

FPT_solver::FPT_solver(
    const Data& _data, 
    State_graph& _SG, 
    const Criteres& _critere, 
    double _time_limit
): data(_data), SG(_SG), critere(_critere), time_limit(_time_limit), found_solution(true) 
{
    timer.start_timer(); 
    // Initialisation de la racine (l'état 0)
    L.push(0); 
    best_dist.push_back(0); 
    pred_in_pcc.push_back({-1, -1});
}

void FPT_solver::compute_cut_set(
    const std::vector<int>& cand, 
    int& cut_set_size, 
    std::vector<uint8_t>& cut_set, 
    std::vector<int>& hors_cut_set
) const 
{
    // On se rappelle que pour calculer le cut-set, il faut déterminer les éléments 
    // qui sont pas dans N_G^+(cand). On met tous les éléments à 1. On parcours N_G^+(cand)
    // grâce à la transitive closure et on met 0 pr chq sommet qu'on rencontre. 

    for(const int c : cand) { // pr chq candidat 
        for(int i = 0; i < data.dag_size; ++i) { // pr chq noeud du graphe
            if(data.TC[c][i] && cut_set[i]) { // si c->i && on a pas encore vu i 
                cut_set[i] = 0; // si c -> i, i n'est pas dans le cut-set 
                hors_cut_set.push_back(i); 
            }
        }
    }

    cut_set_size = data.dag_size - (int)hors_cut_set.size(); 
}

std::vector<int> FPT_solver::rebuild_opt_order() const 
{
    std::vector<int> ordre_topo; 
    int curr_node = (int)pred_in_pcc.size() - 1; // on récup le dernier ens candidat ( {t} )
    while(curr_node != 0) {
        ordre_topo.push_back(pred_in_pcc[curr_node].second); 
        curr_node = pred_in_pcc[curr_node].first; 
    }
    std::reverse(ordre_topo.begin(), ordre_topo.end()); 
    return ordre_topo;  
}

bool FPT_solver::try_eval_time_limit(int iteration_count) 
{
    if(iteration_count % 10000 != 0) 
        return true; 
    double temps_courant = timer.get_temps_passe(); 
    if(temps_courant >= time_limit) {
        this->found_solution = false; 
        return false; 
    }
    return true; 
}

void FPT_solver::solve() 
{   
    int iteration_count = 0; 
    std::vector<int> C2; // pré-allocation 
    C2.reserve(data.dag_size); 

    while(!L.empty())
    {
        iteration_count++; 
        if(!try_eval_time_limit(iteration_count)) // si false -> on doit arreter 
            break; 
        
        int C_ID = L.front(); 
        L.pop(); 
        std::vector<int> C = SG.get_cand(C_ID); 
        int cut_set_size = 0; // 0 par défaut, on va le régler dans compute_cut_set
        std::vector<uint8_t> cut_set(data.dag_size, 1); 
        std::vector<int> hors_cut_set; 
        compute_cut_set(C, cut_set_size, cut_set, hors_cut_set);
        
        // vérification nouveau layer ? 
        // (un layer est un ensemble de cands dont les cut-set associés sont de mm taille)
        if(cut_set_size > SG.curr_layer_size) {
            SG.purge_layer(); // nettoyage 
            SG.curr_layer_size++; // on commence un nouveau layer de taille +1
        }

        SG.curr_layer_IDs.push_back(C_ID); 
        
        for(int i = 0; i < (int)C.size(); ++i) // pr chq candidat de C
        { 
            C2 = C; 
            C2[i] = C2.back(); 
            C2.pop_back();
            int curr_c = C[i]; // copie c 
            cut_set[curr_c] = 1; // le candidat rentre dans le cutset 

            for(int u : data.dag[curr_c]) { // on ajt tt les succs de c qui n'ont plus de pred hors cutset
                if(is_included(data.reverse_dag[u], cut_set, -1)) 
                    C2.push_back(u); // -1 signifie qu'on regarde la simple inclusion de r-dag dans e-c-s
            }   

            sort(C2.begin(), C2.end()); // tri ordre croissant 
            if(C2.size() == 0) continue; 

            keyHash C2_hash = compute_cand_hash(C2, data.node_to_hash); 
            int C2_ID = SG.is_cand_in_SG(C2, C2_hash); 

            if(C2_ID == -1) // (l.10) (si il vaut -1 c'est qu'on a pas trouvé d'ID pr ce hash)
            { 
                SG.add_cand_to_SG(C2, C2_hash); 
                C2_ID = (int)SG.ID_to_cands.size()-1; // on vient de l'ajouter, son ID est le dernier index 
                L.push(C2_ID); // ajout à FIFO
                int C_weight = SG.weights[C_ID]; 
                int C2_weight = critere.compute_weight(C_weight, curr_c, cut_set); // cut_set est le cut-set de C2 
                SG.set_weight(C2_ID, C2_weight); // on calcule le poids de C2 
                best_dist.push_back(std::numeric_limits<int>::max()); // inf par défaut 
                pred_in_pcc.push_back({-1,-1}); // ajout d'un sommet defaut pour garder pred_in_pcc bien indéxé 
            }

            // SG.add_arc_from_C1_to_C2(C_ID, C2_ID); 

            update_distance_label(C_ID, C2_ID, curr_c); // maj du label de distance 

            // on fini la boucle sur curr_c 
            // -> on l'enlève du cut-set et passe au prochain candidat 
            cut_set[curr_c] = 0; 
        }
    }

    // récupération des résultats 
    this->total_time = timer.get_temps_passe();  
}

SolverResults FPT_solver::get_results() const 
{
    SolverResults res;
    res.instance_name = this->data.instance_name; 
    res.dag_size = this->data.dag_size; 
    res.degeneracy = this->data.degenerascy;  
    res.found_solution = this->found_solution; 
    res.total_time = this->total_time; 
    res.nb_cands = this->SG.ID_to_cands.size(); 
    if(found_solution) {
        res.optimal_value = best_dist.back(); 
        res.optimal_order = rebuild_opt_order(); 
    }
    return res; 
}
