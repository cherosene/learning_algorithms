CC = g++
INCLUDEDIR = include
EXAMPLEDIR = examples
SRCDIR = src
CFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR) -c
LFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR)
HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
CPP = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(notdir $(CPP:.cpp=.o))
TEST = test.cpp
MAIN_EXE = $(MAIN:.cpp=)
TEST_EXE = $(TEST:.cpp=)


all: test
	$(MAKE) test
	$(MAKE) clean

compile: $(HEADERS) $(CPP)
	$(CC) $(CFLAGS) $(CPP)
	
test:
	#$(MAKE) compile
	$(CC) $(LFLAGS) $(TEST) -o $(TEST_EXE)
	
rps:
	$(CC) $(LFLAGS) $(EXAMPLEDIR)/rps.cpp -o rps
	$(MAKE) almostclean
	
almostclean:
	rm -f $(OBJ)
	
clean:
	$(MAKE) almostclean
	rm -f $(TEST_EXE) $(EXE)