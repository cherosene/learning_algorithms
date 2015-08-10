#include <ctime>
#include <time.h>
#include <iostream>
#include "utility.h"
#include "QLGameObject.h"
#include "Scopa.h"
#include "ScopaStates.h"

int main (int argc, char const* argv[])
{
    std::vector<stateType> states = generateStates();
    saveStates(states, "states.bin");
    
    return 0;
}