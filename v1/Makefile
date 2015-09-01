CC = g++

INCLUDEDIR = include
EXAMPLEDIR = examples
SRCDIR = src
RPSDIR = $(EXAMPLEDIR)/rock_paper_scissors
CARDSDIR = $(EXAMPLEDIR)/cards

CFLAGS = -Wall -pedantic -g -O0 -std=c++11 -c
LFLAGS = -Wall -pedantic -g -O0 -std=c++11

SDL_INCLUDE = -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers
SDL_CFLAGS = $(CFLAGS) $(SDL_INCLUDE)
SDL_LFLAGS = $(LFLAGS) -F/Library/Frameworks $(SDL_INCLUDE) -framework SDL2 -framework SDL2_image

HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
CPP = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(notdir $(CPP:.cpp=.o))
TEST = test.cpp
MAIN_EXE = $(MAIN:.cpp=)
TEST_EXE = $(TEST:.cpp=)

RPS = $(RPSDIR)/rps.cpp
RPS_EXE = $(notdir $(RPS:.cpp=))

CARDSOBJ_AUX = utility.o Card.o CardGroup.o Scopa.o
CARDSOBJ = $(CARDSOBJ_AUX:%=$(CARDSDIR)/%)
STATEGENERATOR = $(CARDSDIR)/scopaStateGenerator.cpp
MVSM = $(CARDSDIR)/mvsm.cpp
READQLTABLE = $(CARDSDIR)/readQLTable.cpp
STATEGENERATOR_EXE = $(notdir $(STATEGENERATOR:.cpp=))
MVSM_EXE = $(notdir $(MVSM:.cpp=))
READQLTABLE_EXE = $(notdir $(READQLTABLE:.cpp=))
HVSM = $(CARDSDIR)/hvsm.cpp
HVSM_EXE = $(notdir $(HVSM:.cpp=))

EXE = $(MAIN_EXE) $(TEST_EXE) $(RPS_EXE) $(STATEGENERATOR_EXE) $(MVSM_EXE) $(READQLTABLE_EXE) $(HVSM_EXE)

.PHONY: all compile almostclean clean

all: $(HVSM_EXE)

compile: $(CPP)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -I$(CARDSDIR) $(CPP)
	
test: test.cpp
	# $(MAKE) compile
	$(CC) $(LFLAGS) $(TEST) -o $(TEST_EXE)
	
$(RPS_EXE):
	$(CC) $(LFLAGS) $(RPS) -o $(RPS_EXE)
	
hvsh:
	cd $(CARDSDIR); $(MAKE) hvsh
	
$(MVSM_EXE):
	cd $(CARDSDIR); $(MAKE) compile
	$(CC) $(SDL_LFLAGS) -I$(INCLUDEDIR) -I$(CARDSDIR) $(CARDSOBJ) $(MVSM) -o $(MVSM_EXE)
	
$(HVSM_EXE):
	cd $(CARDSDIR); $(MAKE) compile
	$(CC) $(SDL_CFLAGS) -I$(INCLUDEDIR) -I$(CARDSDIR) -c src/scopaQLUtility.cpp
	$(CC) $(SDL_LFLAGS) -I$(INCLUDEDIR) -I$(CARDSDIR) $(CARDSOBJ) scopaQLUtility.o $(HVSM) -o $(HVSM_EXE)
	
$(READQLTABLE_EXE):
	cd $(CARDSDIR); $(MAKE) compile
	$(CC) $(SDL_LFLAGS) -I$(CARDSDIR) $(CARDSOBJ) $(READQLTABLE) -o $(READQLTABLE_EXE)
	
$(STATEGENERATOR_EXE):
	cd $(CARDSDIR); $(MAKE) compile
	$(CC) $(SDL_LFLAGS) -I$(CARDSDIR) $(CARDSOBJ) $(STATEGENERATOR) -o $(STATEGENERATOR_EXE)
	
almostclean:
	rm -f $(OBJ)
	
clean: almostclean
	rm -f $(EXE)