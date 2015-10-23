CC = g++
CFLAGS = -Wall --pedantic -g -O0 -std=c++11 -c
LFLAGS = -Wall --pedantic -g -O0 -std=c++11

INCLUDE_DIR = -Iinclude
SOURCE_DIR	= src
SOURCE_CPP	= $(wildcard $(SOURCE_DIR)/*.cpp)
SOURCE_OBJ	= $(notdir $(SOURCE_CPP:.cpp=.o))

SDL_INCLUDE = -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers
SDL_CFLAGS = $(CFLAGS) $(SDL_INCLUDE)
SDL_LFLAGS = $(LFLAGS) -F/Library/Frameworks $(SDL_INCLUDE) -framework SDL2 -framework SDL2_image

EXAMPLES_DIR = examples

RPS_DIR = $(EXAMPLES_DIR)/rock_paper_scissors
RPS = $(RPS_DIR)/rps.cpp
RPS_EXE = $(notdir $(RPS:.cpp=))

CARDS_DIR = $(EXAMPLES_DIR)/cards
CARDS_INCLUDE_DIR = -I$(CARDS_DIR)/include
CARDS_SRC_DIR = $(CARDS_DIR)/src
CARDS_CPP = $(wildcard $(CARDS_SRC_DIR)/*.cpp)
CARDS_OBJ = $(notdir $(CARDS_CPP:.cpp=.o))
CARDS_HVSH = $(CARDS_DIR)/human_vs_human.cpp
CARDS_HVSH_EXE = $(notdir $(CARDS_HVSH:.cpp=))
CARDS_SCOPA_GAME = $(CARDS_DIR)/scopaGame.cpp
CARDS_SCOPA_GAME_EXE = $(notdir $(CARDS_SCOPA_GAME:.cpp=))

PREY_DIR			= $(EXAMPLES_DIR)/prey
PREY_INCLUDE_DIR	= -I$(PREY_DIR)/include
PREY_SRC_DIR		= $(PREY_DIR)/src
PREY_CPP			= $(wildcard $(PREY_SRC_DIR)/*.cpp)
PREY_OBJ			= $(notdir $(PREY_CPP:.cpp=.o))
PREY_MAIN			= $(PREY_DIR)/prey.cpp
PREY_EXE			= $(notdir $(PREY_MAIN:.cpp=))
PREY_TRAINING_MAIN	= $(PREY_DIR)/preyTraining.cpp
PREY_TRAINING_EXE	= $(notdir $(PREY_TRAINING_MAIN:.cpp=))
QL_PREY_MAIN		= $(PREY_DIR)/qlPrey.cpp
QL_PREY_EXE			= $(notdir $(QL_PREY_MAIN:.cpp=))

EXE = test $(RPS_EXE) $(CARDS_HVSH_EXE) $(CARDS_SCOPA_GAME_EXE)

.PHONY: all preyobj

all: qlPrey
	
test: preyobj
	$(CC) $(SDL_LFLAGS) $(PREY_INCLUDE_DIR) $(INCLUDE_DIR) $(PREY_OBJ) $(SOURCE_OBJ) test.cpp -o test

$(RPS_EXE):
	$(CC) $(LFLAGS) $(INCLUDE_DIR) $(RPS) -o $(RPS_EXE)
	
$(CARDS_HVSH_EXE):
	$(CC) $(SDL_CFLAGS) $(CARDS_INCLUDE_DIR) $(CARDS_CPP)
	$(CC) $(SDL_LFLAGS) $(CARDS_INCLUDE_DIR) $(CARDS_OBJ) $(CARDS_HVSH) -o $(CARDS_HVSH_EXE)
	$(MAKE) almostclean
	
$(CARDS_SCOPA_GAME_EXE):
	$(CC) $(SDL_CFLAGS) $(CARDS_INCLUDE_DIR) $(CARDS_CPP)
	$(CC) $(SDL_LFLAGS) $(INCLUDE_DIR) $(CARDS_INCLUDE_DIR) $(CARDS_OBJ) $(CARDS_SCOPA_GAME) -o $(CARDS_SCOPA_GAME_EXE)
	$(MAKE) almostclean
	
preyobj:
	$(CC) $(SDL_CFLAGS) $(PREY_INCLUDE_DIR) $(INCLUDE_DIR) $(PREY_CPP) $(SOURCE_CPP)
	
$(PREY_EXE): preyobj
	$(CC) $(SDL_LFLAGS) $(PREY_INCLUDE_DIR) $(INCLUDE_DIR) $(PREY_OBJ) $(SOURCE_OBJ) $(PREY_MAIN) -o $(PREY_EXE)
	
$(PREY_TRAINING_EXE): preyobj
	$(CC) $(SDL_LFLAGS) $(PREY_INCLUDE_DIR) $(INCLUDE_DIR) $(PREY_OBJ) $(SOURCE_OBJ) $(PREY_TRAINING_MAIN) -o $(PREY_TRAINING_EXE)
	
$(QL_PREY_EXE): preyobj
	$(CC) $(SDL_LFLAGS) $(PREY_INCLUDE_DIR) $(INCLUDE_DIR) $(PREY_OBJ) $(SOURCE_OBJ) $(QL_PREY_MAIN) -o $(QL_PREY_EXE)
	
almostclean:
	rm -rf *.o *.dSYM
	
cleanexe:
	rm -f $(EXE)
	
clean: almostclean cleanexe