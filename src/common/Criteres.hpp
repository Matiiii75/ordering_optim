#pragma once 

#include "Data.hpp"

struct Criteres 
{
    virtual ~Criteres() = default; // destructeur 

    /**
     * @brief Calcule le poids d'un ensemble candidats en fonction d'un ens. cand. 
     * prédécesseur et de l'élément candidat ajouté 
     * @param pred_cand_set_weight le poids d'un predecesseur 
     * @param cand_c le candidat ajouté
     * @param cut_set nécéssaire pour la maj de DSC (mais pas pour CW)
     */
    virtual int compute_weight(
        int pred_cand_set_weight, 
        int cand_c, 
        const std::vector<uint8_t>& cut_set
    ) const = 0; 

    /**
     * @brief calcul la distance optimale depuis un prédécesseur 
     * @param best_dist_pred le label de distance du pred
     * @param weight_pred le poids du pred dans SG 
     */
    virtual int compute_dist_from_pred(
        int best_dist_pred, 
        int weight_pred
    ) const = 0;  

    /**
     * @brief Calcule la valeur objectif étant donné un ordre topologique 
     * @param ordre_topo ordre topologique qu'on évalue 
     */
    virtual int compute_objective_value(
        const std::vector<int>& ordre_topo
    ) const = 0; 

    /**
     * @brief Checker qui évalue la validité d'un ordre et de sa valeur associée 
     * @param ordre_topo ordre topologique à évaluer 
     * @param value_found la valeur objectif trouvée 
     */
    virtual bool checker(
        const std::vector<int>& ordre_topo, 
        int value_found
    ) const = 0; 

    // méthode qui permet de connaitre le critere 
    // renvoie "DSC" ou "CW"
    virtual std::string get_name() const = 0; 

    virtual std::unique_ptr<Criteres> clone(const Data& data) const = 0; 
}; 


