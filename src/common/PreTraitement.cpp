#include "PreTraitement.hpp"

std::vector<std::vector<int>> PreTraitement::compute_composantes()
{
    std::vector<std::vector<int>> composantes; 
    std::vector<int> to_visit; 
    for(int u = 0; u < data.dag_size; ++u) {
        if(u == data.s || u == data.t) continue; 
        to_visit.push_back(u); 
    }
    std::vector<uint8_t> visited(data.dag_size, 0); 
    for(int u : to_visit)
    {
        if(visited[u]) continue; 
        std::vector<int> composante; 
        visited[u] = 1; // retenir qu'on l'a visité 
        std::queue<int> fifo_list; 
        fifo_list.push(u);

        while(!fifo_list.empty())
        {
            int curr = fifo_list.front(); 
            fifo_list.pop(); 
            composante.push_back(curr);
            
            for(int curr_succ : data.dag[curr]) { 
                if(curr_succ == data.t || visited[curr_succ]) continue; 
                visited[curr_succ] = 1; 
                fifo_list.push(curr_succ); // ajt a la fifo pr traiter ses voisins 
            }
            for(int curr_preds : data.reverse_dag[curr]) {
                if(curr_preds == data.s || visited[curr_preds]) continue;  
                visited[curr_preds] = 1; 
                fifo_list.push(curr_preds); 
            }
        }
        int comp_size = (int)composante.size(); 
        if(comp_size == 1) {
            this->one_node_comp.push_back(composante[0]);
        }
        if(comp_size == 2) {
            int u = composante[0]; 
            int v = composante[1]; 
            two_nodes_comp.push_back({u,v}); 
        }
        if(comp_size > 2) {
            composantes.push_back(composante); 
        }
    }
    return composantes; 
}

Dag PreTraitement::compute_induced_dag(const std::vector<int>& composante)
{
    int new_dag_size = (int)composante.size()+2; // car on ajt s & t 
    int initial_puit = data.t; 
    int source = 0; 
    int puit = new_dag_size-1; // puit du nv dag 

    std::vector<int> new_to_old(new_dag_size); // new_to_old[i] = j -> j devient i 
    std::unordered_map<int,int> old_to_new; // otn[i] = j -> i est devenu j 

    new_to_old[source] = source; 
    old_to_new[source] = source;    // les nv s & t sont associés à eux-même
    new_to_old[puit] = puit;        
    old_to_new[puit] = puit; 

    int new_next_dispo = 1; // valeur du prochain nv noeud dispo
    for(int u : composante) {
        new_to_old[new_next_dispo] = u; 
        old_to_new[u] = new_next_dispo; 
        new_next_dispo++; // on passe au prochain 
    }

    // création du sous graphe 

    Dag new_dag(new_dag_size);
    std::vector<bool> has_0_pred(new_dag_size, true); // true -> degré entrant nul
    
    for(int u : composante) { 
        int new_u = old_to_new.at(u); // on récup sa nv valeur
        for(int u_neigh : this->data.dag[u]) { 
            if(u_neigh == initial_puit) continue; 
            int new_neigh_u = old_to_new.at(u_neigh); // nv valeur du voisin
            has_0_pred[new_neigh_u] = false; 
            new_dag[new_u].push_back(new_neigh_u); 
        }
    }
    // connecter source et puit dans le nv dag 
    for(int u = 1; u < puit; ++u) { 
        if(has_0_pred[u]) 
            new_dag[source].push_back(u); 
        if((int)new_dag[u].size() == 0) { 
            new_dag[u].push_back(puit);   
        }
    }

    this->all_map_new_to_old.push_back(new_to_old); 
    return new_dag; 
}

void PreTraitement::compute_sub_instances()
{
    std::vector<std::vector<int>> composantes; 
    composantes = compute_composantes();
    for(const auto& composante : composantes) {
        Dag sub_dag = compute_induced_dag(composante); 
        Data sub_instance(sub_dag); 
        this->sub_instances.push_back(sub_instance); 
    }
}

