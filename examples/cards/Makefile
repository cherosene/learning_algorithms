CXX = g++
INCLUDEDIRS = -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_image.framework/Headers
CXXFLAGS = -Wall --pedantic $(INCLUDEDIRS) -c -std=c++11
LDFLAGS = -F/Library/Frameworks $(INCLUDEDIRS) -framework SDL2 -framework SDL2_image
HEADERS = utility.h Card.h CardGroup.h Scopa.h
CPP = $(HEADERS:.h=.cpp)
OBJ = $(HEADERS:.h=.o)
EXE = hvsh

all: $(EXE)
	
compile: $(HEADERS) $(CPP)
	$(CXX) $(CXXFLAGS) $(CPP)

$(EXE): $(EXE).o compile
	$(CXX) $(LDFLAGS) $(OBJ) $< -o $@

$(EXE).o: $(EXE).cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)