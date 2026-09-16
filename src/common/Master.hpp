#pragma once 

#include "Data.hpp"
#include "Common.hpp"
#include "FPT_solver.hpp"
#include "ResultsLogs.hpp"

struct Master
{
    const Data& data; 
    double time_limit; 
    bool write_results;
    const Criteres* critere; // le critère à optimiser (si pretraite avec FPT) 

    Master(
        const Data& _data,   
        double _time_limit, 
        bool _write_results,
        const Criteres* _critere = nullptr // par défaut, pointeur null
    ): data(_data), time_limit(_time_limit), write_results(_write_results), critere(_critere) {}

    Master(
        const Data& _data
    ): data(_data) {}

    void solve_FPT_pre_traitement() const; 
    void solve_FPT() const; 
    void solve_CW2() const; 
    void solve_CW3() const; 
    void solve_DSC1() const; 
    void solve_DSC2() const; 
    void solve_DSC3() const; 
}; 