#pragma once 

#include "Data.hpp"

struct SolverResults
{
    std::string instance_name; 
    int dag_size; 
    int degeneracy; 
    bool found_solution = 0;
    double total_time = -1.0; 
    int optimal_value = -1; 
    int nb_cands = -1; 
    std::vector<int> optimal_order;
    double gap = -1.0;
    double best_bound = -1.0; 
    int nb_sub_pb_solved = -1; // nombre de composantes de taille > 2 détéctées dans l'instance 
    int nb_total_composantes = -1; 
    SolverResults() {}
}; 

/**
 * @brief fonction qui teste l'inclusion de v1 dans v2 - {t} (car t compte pas dans l'objectif)
 * @param v1 le vecteur qui est inclus 
 * @param v2 vecteur de uint8_t où v2[u] = 1 si u /in cut-set, 0 sinon  
 * @param t le noeud puit qu'on ne considère pas dans le calcul
 * @return true si inclus, false sinon
 * @note on mettra t = -1 par défaut pour dire qu'on regarde juste une inclusion sans chercher à exclure le puit t 
 */
bool is_included(const std::vector<int>& v1, const std::vector<uint8_t>& v2, int t); 

/**
 * @brief détermine si il existe un successeur de gamma qui n'est pas dans le cut set
 * (l.8,9 - algo 2)
 * @param succ_gamma vecteur contenant les successeurs de gamma 
 * @param t le noeud puit du dag (à ne pas considérer dans les calculs justement)
 * @param cut_set vecteur de uint8_t où v2[u] = 1 si u /in cut-set, 0 sinon
 * @return vrai si il existe un successeur, faux sinon 
 * @note version où le cut_set est un unordered_set
 */
bool is_disjoint(const std::vector<int>& succ_gamma, int t, 
    const std::vector<uint8_t>& cut_set); 

