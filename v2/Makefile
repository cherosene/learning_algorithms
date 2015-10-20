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

EXE = test $(RPS_EXE) $(CARDS_HVSH_EXE) $(CARDS_SCOPA_GAME_EXE)

.PHONY: all

all: test
	
test:
	$(CC) $(CFLAGS) $(INCLUDE_DIR) $(SOURCE_CPP)
	$(CC) $(LFLAGS) $(INCLUDE_DIR) $(SOURCE_OBJ) test.cpp -o test

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
	
almostclean:
	rm -rf *.o *.dSYM
	
clean: almostclean
	rm -f $(EXE)