#ifndef AXON_H
#define AXON_H

#include "Neuron.h"

class Axon {
    
public:
    const bool isBias;
    float weight;
    Neuron* const source;
    Neuron* const target;
    
public:
    Axon(float initialWeight, Neuron& s, Neuron& t);
    Axon(float initialWeight, Neuron& t);
    
    void giveOut();
    void giveErr();
    
    void update(float alpha);
    
};


#endif // AXON_H