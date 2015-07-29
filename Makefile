CC = g++
INCLUDEDIR = include
SRCDIR = src
CFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR) -c
LFLAGS = -Wall -pedantic -std=c++11 -I$(INCLUDEDIR)
HEADERS = $(wildcard $(INCLUDEDIR)/*.h)
CPP = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(notdir $(CPP:.h=.o))
MAIN = main.cpp
TEST = test.cpp
MAIN_EXE = $(MAIN:.cpp=)
TEST_EXE = $(TEST:.cpp=)


all: test
	$(MAKE) test
	$(MAKE) clean

compile: $(HEADERS) $(CPP)
	$(CC) $(CFLAGS) $(CPP)
	
test:
	$(MAKE) compile
	$(CC) $(LFLAGS) $(TEST) -o $(TEST_EXE)
	
almostclean:
	rm -f $(OBJ)
	
clean:
	make almostclean
	rm -f $(TEST_EXE) $(EXE)