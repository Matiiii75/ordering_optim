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


/*
Main du problème. 
Prend 3 arguments : 
1 -> instance du problème
2 -> choix de l'algo 
3 -> choix d'écrire le résultats dans un fichier .txt
Pour lancer le code : 
./prog [instances/inst.txt] [choix_algo] [choix_ecriture]
Détail des paramètres : 
0 -> DSC FPT 
1 -> CW FPT 
2 -> PL DSC1 
3 -> PL DSC2 
4 -> PL DSC3
5 -> PL CW2 
6 -> PL CW3
7 -> FPT DSC + Pré-traitement
*/

int main(int argc, char* argv[]) 
{   
    if(argc != 4) std::cout << "erreur nb args" << std::endl;

    std::string instance_file = argv[1]; 
    int choice_algo = atoi(argv[2]); 
    int write_results = atoi(argv[3]); 
    double time_limit = 600.00; 
    Data data(instance_file);

    if(choice_algo == 0) // DSC FPT
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT(); 
    }
    if(choice_algo == 1) // CW FPT 
    {
        DagCutwidth critere_CW(data); 
        Master master(data, time_limit, write_results, &critere_CW); 
        master.solve_FPT(); 
    }
    if(choice_algo == 2) // PL DSC1
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC1(); 
    }
    if(choice_algo == 3) // PL DSC2
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC2();
    }
    if(choice_algo == 4) // PL DSC3
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC3();  
    }
    if(choice_algo == 5) // PL CW2
    {
        Master master(data, time_limit, write_results); 
        master.solve_CW2();
    }
    if(choice_algo == 6) // PL CW3
    {
        Master master(data, time_limit, write_results); 
        master.solve_CW3(); 
    }
    if(choice_algo == 7) // pré-traitement DSC
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT_pre_traitement(); 
    }

    return 0; 
}