#include "Master.hpp"

void Master::solve_FPT_pre_traitement() const 
{}

void Master::solve_FPT() const 
{
    if(critere == nullptr)
        throw std::invalid_argument("Master::solver_FPT -> critere == nullptr ...");

    SolverResults res; 
    ResultsLogs logs; 
    bool display_order = false; 
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