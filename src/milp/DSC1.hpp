#pragma once 

#include "Milp.hpp"

struct DSC1 : Milp
{   
    DSC1(const Data& _data, double _time_limit)
        : Milp(_data, _time_limit) {} // il appelle le constructeur de la classe mère
    
    virtual void solve(bool verbose= false) override; 

    virtual SolverResults get_results() override;  
}; 

