#pragma once 

#include "Data.hpp"

// keyHasher custom pour l'unordered_map de la struct "state_graph"
// lui permet de hasher les clés
struct KeyHasher { 

    std::size_t operator()(const keyHash& k) const {
        return static_cast<std::size_t>(k.l); 
    }

}; 


// operateur de comparaison custom entre deux clé pour l'um de struct "state_graph" 
struct KeyEqual {

    bool operator()(const keyHash& k1, const keyHash& k2) const {
        return k1.l == k2.l && k1.r == k2.r; 
    }

}; 


struct State_graph 
{

    const Data& data; 
    std::vector<std::vector<int>> ID_to_cands; 
    std::unordered_map<keyHash, std::vector<int>, KeyHasher, KeyEqual> hash_to_ID; 
    std::vector<std::vector<int>> SG; 
    std::vector<int> weights; 
    int curr_layer_size; 
    std::vector<int> curr_layer_IDs;    

    State_graph(const Data& _data); 

    /**
     * @brief calcule le premier ensemble candidat
     * @note il faut trouver tous les successeurs directs de s qui
     *  n'ont que s en prédécesseur direct
     * @warning s'occupe du tri de first_cand en interne 
     */
    std::vector<int> compute_first_cand() const; 

    /**
     * @brief vérifie si un ensemble candidat est déja dans SG 
     * @param cand ensemble candidat à vérifier 
     * @param cand_hash le hash de l'ensemble candidat (pré-calculé)
     * @return l'ID si cand est déjà dans SG, -1 sinon 
     * @warning cand doit être trié par ordre croissant
     */
    int is_cand_in_SG(const std::vector<int>& cand, const keyHash& cand_hash) const ; 

    /**
     * @brief ajoute un ensemble candidat au graphe d'états 
     * @param cand l'ensemble candidat à ajouter 
     * @param cand_hash le hash pré-calculé de cand 
     * @warning on doit rentrer dans cette fonction QUE SI cand n'est pas déjà dans SG 
     * @note la fonction 
     */
    void add_cand_to_SG(const std::vector<int>& cand, const keyHash& cand_hash); 

    /**
     * @brief ajoute l'arc (C1,C2) graphe d'états 
     * @param cand1_ID le sommet départ 
     * @param cand2_ID le sommet arrivée  
     * @warning On doit avoir au préalable vérifié que cand1_ID et cand2_ID sont bien dans SG et ont bien le bon ID associé 
     */
    void add_arc_from_C1_to_C2(int cand1_ID, int cand2_ID);   

    /**
     * @brief getter qui renvoie l'ensemble candidat associé à un ID 
     * @param ID l'ID voulu
     * @warning devrait pas arriver mais si l'index ID n'existait pas, lèvera une erreur 
     * @note ref const sur le type de renvoie pour éviter copie puisqu'on va juste le parcourir 
     */
    const std::vector<int>& get_cand(int ID) const; 

    /**
     * @brief set le poids d'un ID 
     * @param ID 
     * @param w le poids de ID 
     * @note lève une erreur si ID >= taille(weights)
     */
    void set_weight(int ID, int w); 

    void purge_layer(); 

    /* AFFICHAGES */

    void display_SG() const; // méthode d'affichage de SG dans le terminal 

    void display_SG_detail() const; // méthode d'affichage de SG avec les relations entre ensembles 

    void display_weights() const; // afficher les poids de chaque ensemble candidat

}; 