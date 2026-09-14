# 1. Variables de configuration
CXX = g++
CXXFLAGS = -O3 -g -std=c++17 -Wall -Wextra 

# --- AJOUT CRUCIAL : Indiquer les dossiers contenant les .hpp ---
CXXFLAGS += -Isrc/common -Isrc/fpt -Isrc/milp -Isrc/meta 

# --- DÉTECTION DU SYSTÈME ET DU MODULE GUROBI ---
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    GUROBI_DIR = /Library/gurobi1302/macos_universal2
    GUROBI_INC = -I$(GUROBI_DIR)/include
    GUROBI_LIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi130 -Wl,-rpath,$(GUROBI_DIR)/lib
else
    GUROBI_DIR = $(GUROBI_HOME)
    GUROBI_INC = -I$(GUROBI_DIR)/include
    GUROBI_LIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi130 -Wl,-rpath,$(GUROBI_DIR)/lib
endif

CXXFLAGS += $(GUROBI_INC)
# --- FIN CONFIGURATION GUROBI ---

# Un seul exécutable unifié
EXEC_CPP = prog

# 2. Séparation stricte avec les chemins complets
SRC_COMMON = src/common/Common.cpp \
             src/common/Data.cpp \
             src/common/Timer.cpp \
             src/common/ResultsLogs.cpp \
             src/common/DagSumCut.cpp \
             src/common/DagCutwidth.cpp \
             src/fpt/State_graph.cpp \
             src/fpt/FPT_solver.cpp \
             src/milp/DSC1.cpp \
             src/milp/DSC2.cpp \
             src/milp/DSC3.cpp \
             src/milp/CW2.cpp \
             src/milp/CW3.cpp

SRC_MAIN_CPP = src/main.cpp

# 3. Transformation des .cpp en .o
OBJ_COMMON   = $(SRC_COMMON:.cpp=.o)
OBJ_MAIN_CPP = $(SRC_MAIN_CPP:.cpp=.o)

ALL_OBJ = $(OBJ_COMMON) $(OBJ_MAIN_CPP)

# 4. Règles de construction
all: $(EXEC_CPP)

# Programme principal unifié (FPT + Gurobi)
$(EXEC_CPP): $(OBJ_MAIN_CPP) $(OBJ_COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GUROBI_LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 5. Nettoyage
clean:
	rm -f $(ALL_OBJ) $(EXEC_CPP) gurobi.log

fclean: clean
	rm -f $(EXEC_CPP) gurobi.log

re: fclean all

.PHONY: all clean fclean re