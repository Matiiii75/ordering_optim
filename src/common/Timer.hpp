#pragma once 

#include <iostream>
#include <chrono>

struct Timer {

    std::chrono::steady_clock::time_point start; 

    // démarrer un minuteur 
    void start_timer(); 

    // récuperer le temps passé depuis le début 
    double get_temps_passe(); 

};  

