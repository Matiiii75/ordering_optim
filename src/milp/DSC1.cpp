#include "DSC1.hpp"

void DSC1::solve(bool verbose)
{
    try {
        // GRBEnv et GRBModel sont déjà des objets RAII (l'API Gurobi gère
        // elle-même la libération des ressources sous-jacentes dans leur
        // destructeur) : pas besoin de new/delete, des objets locaux suffisent.
        GRBEnv env(true);
        env.set(GRB_IntParam_OutputFlag, verbose); // 0 pr désactier les affichages dans le terminal
        env.set(GRB_IntParam_Threads, 1); // FORCER GUROBI A N'UTILISER QU'UN SEUL THREAD PAR INSTANCE
        env.start();

        GRBModel model(env);

        if (time_limit > 0.0) {
            model.set(GRB_DoubleParam_TimeLimit, time_limit);
        }

        int nb_nodes = data.dag_size;
        int source = data.s;
        int puit = data.t;

        std::vector<int> all_pos;
        for (int i = 0; i < nb_nodes - 2; ++i) {
            all_pos.push_back(i);
        }

        std::vector<int> all_nodes;
        for (int i = 0; i < nb_nodes; ++i) {
            if (i != source && i != puit) {
                all_nodes.push_back(i);
            }
        }

        std::vector<std::pair<int, int>> all_arcs;
        for (int i = 0; i < nb_nodes; ++i) {
            for (int j : data.dag[i]) {
                if (i != source && j != puit) {
                    all_arcs.push_back({i, j});
                }
            }
        }

        int last_pos = all_pos.empty() ? -1 : all_pos.back();

        // Matrice de variables (indexée par all_pos et all_nodes)
        std::vector<std::vector<GRBVar>> x(nb_nodes, std::vector<GRBVar>(nb_nodes));
        std::vector<std::vector<GRBVar>> y(nb_nodes, std::vector<GRBVar>(nb_nodes));

        for (int i : all_pos) {
            for (int j : all_nodes) {
                std::string name_x = "x_" + std::to_string(i) + "_" + std::to_string(j);
                x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name_x);

                std::string name_y = "y_" + std::to_string(i) + "_" + std::to_string(j);
                y[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name_y);
            }
        }

        // Objectif
        GRBLinExpr obj = 0;
        for (int i : all_pos) {
            for (int j : all_nodes) {
                obj += y[i][j];
            }
        }
        model.setObjective(obj, GRB_MINIMIZE);

        // Contrainte 1 : Toute position a un job unique
        for (int i : all_pos) {
            GRBLinExpr c1 = 0;
            for (int j : all_nodes) c1 += x[i][j];
            model.addConstr(c1 == 1, "pos_" + std::to_string(i));
        }

        // Contrainte 2 : Tout job a une position unique
        for (int j : all_nodes) {
            GRBLinExpr c2 = 0;
            for (int i : all_pos) c2 += x[i][j];
            model.addConstr(c2 == 1, "node_" + std::to_string(j));
        }

        // Contrainte 3 : Respect de l'ordre imposé par le DAG
        for (int i0 : all_pos) {
            for (auto& arc : all_arcs) {
                int j1 = arc.first;
                int j2 = arc.second;
                GRBLinExpr c3 = 0;
                for (int i = 0; i <= i0; ++i) {
                    c3 += (x[i][j1] - x[i][j2]);
                }
                model.addConstr(c3 >= 0);
            }
        }

        // Contrainte 4 : Définition des variables y
        for (int i0 : all_pos) {
            if (i0 == last_pos) continue;
            for (auto& arc : all_arcs) {
                int j1 = arc.first;
                int j2 = arc.second;
                GRBLinExpr rhs = 0;
                
                for (int i = 0; i <= i0; ++i) {
                    rhs += x[i][j1];
                }
                for (int i = i0 + 1; i <= last_pos; ++i) {
                    rhs += x[i][j2];
                }
                model.addConstr(y[i0][j1] + 1 >= rhs);
            }
        }

        // Résolution
        model.set(GRB_DoubleParam_MIPGap, 0.0); // OPTIMALITÉ STRICTE, on ne s'arrete pas tant que UB != LB
        model.optimize();

        int status = model.get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {

            this->total_time = model.get(GRB_DoubleAttr_Runtime); 

            if (model.get(GRB_IntAttr_SolCount) > 0) {

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

SolverResults DSC1::get_results() 
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



