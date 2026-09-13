#include "CW3.hpp"

void CW3::solve(bool verbose)
{
    try {
        
        GRBEnv env(true);
        env.set(GRB_IntParam_OutputFlag, verbose);
        env.set(GRB_IntParam_Threads, 1); // FORCER GUROBI A N'UTILISER QU'UN SEUL THREAD PAR INSTANCE
        env.start();

        GRBModel model(env);

        if (time_limit > 0.0) {
            model.set(GRB_DoubleParam_TimeLimit, time_limit);
        }

        int nb_nodes = data.dag_size;
        int source = data.s; 
        int puit = data.t; 
        int nb_pos = nb_nodes+1;
        
        std::vector<int> all_pos; // pr stocker toutes les positions [1,n]
        for(int i = 1; i < nb_pos; ++i) all_pos.push_back(i);  

        std::vector<std::pair<int, int>> all_arcs;
        for (int i = 0; i < nb_nodes; ++i) {
            for (int j : data.dag[i]) {
                if (i != source && j != puit) {
                    all_arcs.push_back({i, j});
                }
            }
        }

        // variables 

        // xui = 1 si phi(u) <= i
        std::vector<std::vector<GRBVar>> x(nb_nodes, std::vector<GRBVar>(nb_pos)); 

        for(int u = 0; u < nb_nodes; ++u) {
            for(int i : all_pos) {
                std::string name = "x_" + std::to_string(u) + "_" + std::to_string(i); 
                x[u][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name); 
            }
        }

        // zuvi = 1 si l'arc (u,v) traverse la position i 
        std::map<std::pair<int,int>, std::vector<GRBVar>> z; 
        for(const auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            z[{u,v}].resize(nb_pos); // allouer la taille juste pr cet arc
            for(int i : all_pos) {
                std::string name = "z_" + std::to_string(u) + "_" + std::to_string(v) + std::to_string(i); 
                // on peut déclarer z en CONTINUE 
                z[{u,v}][i] = model.addVar(0.0, 1.0 , 0.0, GRB_CONTINUOUS, name); 
            }
        }

        // variable C 
        GRBVar c = model.addVar(0.0, GRB_INFINITY, 1.0, GRB_CONTINUOUS, "c"); 

        // CONTRAINTES 

        // ces deux contraintes éliminent de la symétrie en fixant direct la position de source et puit 
        model.addConstr(x[source][1] == 1);               // phi(source) = 1
        model.addConstr(x[puit][nb_pos - 2] == 0);         // phi(puit) = nb_nodes (== n)

        // définition de c

        for(int i : all_pos) {
            GRBLinExpr cstr_def_c = 0; 
            for(auto& arc : all_arcs) 
                cstr_def_c += z[arc][i];
            model.addConstr(c >= cstr_def_c);  
        }

        // si xui = 1, alors xu(i+1) = 1 
        
        for(int u = 0; u < nb_nodes; ++u) {
            for(int i = 1; i < nb_pos-1; ++i) {
                model.addConstr(x[u][i] <= x[u][i+1]); 
            }
        }

        // le nombre de sommets placés avant ou sur i vaut i

        for(int i = 1; i < nb_pos; ++i) {
            GRBLinExpr cstr_nb_sommets = 0; 
            for(int u = 0; u < nb_nodes; ++u)
                cstr_nb_sommets += x[u][i]; 
            model.addConstr(cstr_nb_sommets == i); 
        }

        // définition de zuvi 
        for(auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            for(int i = 1; i < nb_pos; ++i) 
                model.addConstr(z[arc][i] >= x[u][i] - x[v][i]); 
        }

        // contraintes d'ordre topologique (u,v) \in A implique phi(u) < phi(v)

        for(auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            for(int i = 2; i < nb_pos; ++i) 
                model.addConstr(x[u][i-1] >= x[v][i]); 
        }

        // Résolution
        model.optimize();

        int status = model.get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {

            this->total_time = model.get(GRB_DoubleAttr_Runtime); 

            if (model.get(GRB_IntAttr_SolCount) > 0) { // si on a trouvé au moins une solution 
                
                this->found_solution = true; 
                this->optimal_value = model.get(GRB_DoubleAttr_ObjVal);
                this->best_bound = model.get(GRB_DoubleAttr_ObjBound);
                this->gap = model.get(GRB_DoubleAttr_MIPGap);

            } else {
                
                try {
                    this->best_bound = model.get(GRB_DoubleAttr_ObjBound); 
                } catch (const GRBException& e) {
                    std::cout << "best bound : non dispo, arret trop précoce"; 
                }
            }
        } else if (status == GRB_INFEASIBLE) {
            std::cout << "[Gurobi] ERREUR : Le modele est infaisable.\n";
        }

        // Pas de nettoyage manuel à faire : model puis env (ordre inverse de
        // construction) sont détruits automatiquement en sortie de portée,
        // que la fonction se termine normalement ou via une exception.

    } catch (const GRBException& e) {
        std::cerr << "\n[CRASH GUROBI] Code " << e.getErrorCode() << " : " << e.getMessage() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[CRASH C++] " << e.what() << "\n";
    }
}

SolverResults CW3::get_results() 
{
    SolverResults res;
    
    res.instance_name = this->data.instance_name; 
    res.dag_size = this->data.dag_size; 
    res.degeneracy = this->data.degenerascy;  
    res.found_solution = this->found_solution; 
    res.total_time = this->total_time;  
    res.optimal_value = (int)this->optimal_value;
    res.gap = this->gap; 
    res.best_bound = this->best_bound; 

    return res; 
}



