#!/bin/bash

# ==============================================================================
# SCRIPT DE BENCHMARK UNIFIÉ - CLUSTER OAR (LIP6)
# ==============================================================================
# SYNTAXE :
#   ./run_experiments.sh [MODE]
#
# MODES DISPONIBLES :
#   fpt_dsc    : FPT exact pour SumCut
#   fpt_cw     : FPT exact pour Cutwidth
#   milp_dsc1  : Gurobi - SumCut (Modèle 1 - Absolu)
#   milp_dsc2  : Gurobi - SumCut (Modèle 2 - Relatif)
#   milp_dsc3  : Gurobi - SumCut (Modèle 3 - Step variables)
#   milp_cw2   : Gurobi - Cutwidth (Modèle 2 - Big M)
#   milp_cw3   : Gurobi - Cutwidth (Modèle 3 - Step variables)
#   fpt_pt_dsc : FPT + pré-traitement pour dagsumcut 
#   fpt_pt_cw  : FPT + pré-traitement pour dagcutwidth 
# ==============================================================================

MODE_CIBLE="${1:-fpt_dsc}" # Mode par défaut

# Chemins et ressources
INST_DIR="/home/periat/ordering_optim/instances"
RES_DIR="/home/periat/ordering_optim/results"
EXEC="/home/periat/ordering_optim/prog"

NB_CORES=16
WRITING_RESULTS=1

# ==============================================================================
# 2. DÉDUCTION DU CHOIX ALGO (argv[2] de ton main.cpp)
# ==============================================================================
CHOICE_ALGO=-1

case "$MODE_CIBLE" in
    fpt_dsc)   CHOICE_ALGO=0 ;;
    fpt_cw)    CHOICE_ALGO=1 ;;
    milp_dsc1) CHOICE_ALGO=2 ;;
    milp_dsc2) CHOICE_ALGO=3 ;;
    milp_dsc3) CHOICE_ALGO=4 ;;
    milp_cw2)  CHOICE_ALGO=5 ;;
    milp_cw3)  CHOICE_ALGO=6 ;;
    fpt_pt_dsc) CHOICE_ALGO=7 ;;
    fpt_pt_cw) CHOICE_ALGO=8 ;; 
    *)
        echo "Erreur : Mode '$MODE_CIBLE' inconnu."
        exit 1
        ;;
esac

# ==============================================================================
# 3. RÉSUMÉ DANS LES LOGS OAR
# ==============================================================================
echo "========================================"
echo " JOB OAR - RECAPITULATIF DE CONFIGURATION"
echo "========================================"
echo "- Date de lancement  : $(date)"
echo "- Node / Host        : $(hostname)"
echo "- Mode selectionne   : $MODE_CIBLE"
echo "- choice_algo associe: $CHOICE_ALGO"
echo "- Coeurs alloues     : $NB_CORES"
echo "========================================"

# ==============================================================================
# 4. ENVIRONNEMENT MODULES & DOSSIERS
# ==============================================================================
if [ -f /etc/profile.d/modules.sh ]; then
    source /etc/profile.d/modules.sh
elif [ -f /usr/share/modules/init/bash ]; then
    source /usr/share/modules/init/bash
fi

module load gurobi/13.0.2
mkdir -p "$RES_DIR"

# ==============================================================================
# 5. FONCTION DE TRAITEMENT D'UNE INSTANCE
# ==============================================================================
run_one_instance() {
    local file="$1"
    local filename=$(basename "$file")
    local n=$(echo "$filename" | cut -d'_' -f1)

    # Filtrage selon la logique de ton ancien script (FPT <= 50, MILP <= 500)
    if (( CHOICE_ALGO == 0 || CHOICE_ALGO == 1 || CHOICE_ALGO == 7 || CHOICE_ALGO == 8)); then
        if (( n > 5000 )); then # ignorer les instances n < 1000
            return 0
        fi
    else
        if (( n != 500 )); then
            return 0
        fi
    fi

    echo "START : $filename ($MODE_CIBLE)"
    
    # Exécution de ton programme unifié (prog_file choice_algo write_results)
    "$EXEC" "$file" "$CHOICE_ALGO" "$WRITING_RESULTS"
    
    echo "DONE  : $filename ($MODE_CIBLE)"
}

export -f run_one_instance
export EXEC CHOICE_ALGO WRITING_RESULTS MODE_CIBLE

# ==============================================================================
# 6. EXECUTION PARALLELE
# ==============================================================================
echo "Demarrage des calculs..."
ls "$INST_DIR"/*.txt | sort -V -t'k' -k2 | parallel -j "$NB_CORES" run_one_instance {}
echo "Fin des calculs : $(date)"