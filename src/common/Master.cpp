#include "Master.hpp"

void Master::solve_FPT_pre_traitement() const 
{
    if(critere == nullptr)
        throw std::invalid_argument("Master::solver_FPT_pre_traitement -> critere == nullptr"); 

    display_execution_choice("FPT-PT", "DSC"); 

    double time_remaining = time_limit; 
    int total_value = 0; 
    int nb_total_cands = 0; 
    std::vector<std::vector<int>> sub_solutions; 
    SolverResults res; 
    ResultsLogs logs; 
    bool found_global_solution = true; 

    PreTraitement pt(data); 
    pt.compute_sub_instances(); 

    for(int i = 0; i < (int)pt.sub_instances.size(); ++i) 
    {
        // on doit créer sub_critere car critere possède la topologie
        // du dag initial et ne pourra donc pas évaluer les coupes proprement. 
        // on créer donc un sous_critere depuis sub_inst
        const Data& sub_inst = pt.sub_instances[i]; 
        auto sub_critere = critere->clone(sub_inst); 
        State_graph sub_SG(sub_inst); 
        FPT_solver sub_solver(sub_inst, sub_SG, *sub_critere, time_remaining);
        sub_solver.solve(); 
        SolverResults sub_res = sub_solver.get_results();
        nb_total_cands += sub_res.nb_cands; 
        time_remaining -= sub_res.total_time; 

        if(sub_res.found_solution) {
            total_value += sub_res.optimal_value; 
            std::vector<int> sub_solution = sub_res.optimal_order; 
            std::vector<int> relabelled_sol;
            relabelled_sol = pt.re_label_sub_solution(sub_solution, i); 
            sub_solutions.push_back(relabelled_sol); 
        } else {
            found_global_solution = false; 
            break;
        }
    }
    total_value += (int)pt.two_nodes_comp.size(); 

    double total_time = time_limit - time_remaining; 

    if(!found_global_solution) // si on a pas trouvé une solution complete
    {
        res = pt.store_results(
            -1, // opt par defaut
            nb_total_cands, 
            total_time, 
            {-1} // global solution par défaut 
        ); 
    } 
    else // si on a trouvé une solution complète
    {
        // reconstruction de la solution : 
        std::vector<int> global_solution; 
        
        for(int node : pt.one_node_comp) // ajt compo 1 sommet
            global_solution.push_back(node); 

        for(auto& nodes : pt.two_nodes_comp) { // ajt compo 2 sommets
            int u = nodes.first; 
            int v = nodes.second; 
            bool u_is_pref_of_v = false; 
            for(int neigh : data.dag[u]) {
                if(neigh == v) {
                    u_is_pref_of_v = true; 
                    global_solution.push_back(u); 
                    global_solution.push_back(v); 
                    break; 
                }
            }
            if(!u_is_pref_of_v) {
                global_solution.push_back(v); 
                global_solution.push_back(u); 
            }
        }

        for(auto& sol : sub_solutions) { // ajt compo > 2 sommets 
            for(int node : sol) 
                global_solution.push_back(node); 
        }

        res = pt.store_results(
            total_value, 
            nb_total_cands, 
            total_time, 
            global_solution
        );  
    }

    logs.display_FPT_pre_traitement(res, display_order); 
    if(found_global_solution)
        critere->checker(res.optimal_order, res.optimal_value);
    if(write_results)
        logs.write_FPT_pre_traitement_results(res, critere->get_name()); 
}

void Master::solve_FPT() const 
{
    if(critere == nullptr)
        throw std::invalid_argument("Master::solver_FPT -> critere == nullptr ...");

    display_execution_choice("FPT", critere->get_name()); 

    SolverResults res; 
    ResultsLogs logs; 
    State_graph SG(data); 
    FPT_solver solver(data, SG, *critere, time_limit); 
    solver.solve(); 
    res = solver.get_results(); 
    logs.display_FPT(res, display_order); 
    if(res.found_solution)
        critere->checker(res.optimal_order, res.optimal_value); 
    if(write_results)
        logs.write_FPT_results(res, critere->get_name()); 
}

void Master::solve_CW2() const 
{
    display_execution_choice("CW2", "CW"); 

    SolverResults res; 
    ResultsLogs logs; 
    CW2 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "CW2"); 
}

void Master::solve_CW3() const 
{
    display_execution_choice("CW3", "CW");

    SolverResults res; 
    ResultsLogs logs; 
    CW3 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "CW3"); 
}

void Master::solve_DSC1() const 
{
    display_execution_choice("DSC1", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC1 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC1"); 
}

void Master::solve_DSC2() const 
{
    display_execution_choice("DSC2", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC2 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC2"); 
}

void Master::solve_DSC3() const 
{
    display_execution_choice("DSC3", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC3 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC3"); 
}

void Master::display_execution_choice(const std::string& algo_type, const std::string& critere) const 
{
    std::cout << "             [-------- ÉXÉCUTION DE "; 
    std::cout << algo_type << " " << critere; 
    std::cout << " --------]" << std::endl;
    std::cout << std::endl;
    std::cout << "[Temps limite]          : " << time_limit << std::endl;
    std::cout << "[Écriture résultats]    : "; 
    if(write_results) std::cout << "activée" << std::endl;
    else std::cout << "désactivée" << std::endl;
    std::cout << std::endl;
}