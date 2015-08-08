CC = g++

INCLUDEDIR = include
EXAMPLEDIR = examples
SRCDIR = src
RPSDIR = $(EXAMPLEDIR)/rock_paper_scissors

CFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR) -c
LFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR)

HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
CPP = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(notdir $(CPP:.cpp=.o))
TEST = test.cpp
RPS = $(RPSDIR)/rps.cpp

MAIN_EXE = $(MAIN:.cpp=)
TEST_EXE = $(TEST:.cpp=)
RPS_EXE = $(notdir $(RPS:.cpp=))
EXE = $(MAIN_EXE) $(TEST_EXE) $(RPS_EXE)

.PHONY: all compile almostclean clean

all: test clean

compile: $(HEADERS) $(CPP)
	$(CC) $(CFLAGS) $(CPP)
	
test: test.cpp
	# $(MAKE) compile
	$(CC) $(LFLAGS) $(TEST) -o $(TEST_EXE)
	
rps:
	$(CC) $(LFLAGS) $(RPS) -o $(RPS_EXE)
	
almostclean:
	rm -f $(OBJ)
	
clean: almostclean
	rm -f $(EXE)