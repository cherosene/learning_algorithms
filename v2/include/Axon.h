#ifndef AXON_H
#define AXON_H

#include "Neuron.h"

class Axon {
    
public:
    float weight;
    Neuron* const source;
    Neuron* const target;
    
public:
    Axon(float initialWeight, Neuron& s, Neuron& t);
    
    void giveOut();
    void giveErr();
    
    void update(float alpha);
    
};


#endif // AXON_H