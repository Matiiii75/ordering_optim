#include <iostream>
#include <string>
#include <exception>

#include "Data.hpp"
#include "Master.hpp"
#include "State_graph.hpp"
#include "DagSumCut.hpp"
#include "DagCutwidth.hpp"
#include "FPT_solver.hpp"
#include "ResultsLogs.hpp"
#include "DSC1.hpp"
#include "DSC2.hpp"
#include "DSC3.hpp"
#include "CW2.hpp"
#include "CW3.hpp"

int main(int argc, char* argv[]) 
{   
    if(argc != 4) std::cout << "erreur nb args" << std::endl;

    std::string instance_file = argv[1]; 
    int choice_algo = atoi(argv[2]); 
    int write_results = atoi(argv[3]); 
    double time_limit = 600.00; 
    Data data(instance_file);
    SolverResults res; 
    ResultsLogs logs;
    bool display_order = false; 

    if(choice_algo == 0) 
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT(); 
    }
    if(choice_algo == 1)
    {
        DagCutwidth critere_CW(data); 
        Master master(data, time_limit, write_results, &critere_CW); 
        master.solve_FPT(); 
    }
    if(choice_algo == 2)
    {
        DSC1 milp(data, time_limit); 
        milp.solve(); 
        res = milp.get_results(); 
        logs.display_Milp(res, display_order); 
        if(write_results)
            logs.write_MILP_results(res, "DSC1"); 
    }
    if(choice_algo == 3) 
    {
        DSC2 milp(data, time_limit); 
        milp.solve(); 
        res = milp.get_results(); 
        logs.display_Milp(res, display_order); 
        if(write_results)
            logs.write_MILP_results(res, "DSC2"); 
    }
    if(choice_algo == 4)
    {
        DSC3 milp(data, time_limit); 
        milp.solve(); 
        res = milp.get_results(); 
        logs.display_Milp(res, display_order);
        if(write_results)
            logs.write_MILP_results(res, "DSC3");  
    }
    if(choice_algo == 5) 
    {
        CW2 milp(data, time_limit); 
        milp.solve(); 
        res = milp.get_results(); 
        logs.display_Milp(res, display_order);
        if(write_results)
            logs.write_MILP_results(res, "CW2"); 
    }
    if(choice_algo == 6) 
    {
        CW3 milp(data, time_limit); 
        milp.solve(); 
        res = milp.get_results(); 
        logs.display_Milp(res, display_order);
        if(write_results)
            logs.write_MILP_results(res, "CW3"); 
    }
    if(choice_algo == 7)
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT_pre_traitement(); 
    }

    return 0; 
}