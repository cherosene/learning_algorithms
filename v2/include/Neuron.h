#ifndef NEURON_H
#define NEURON_H

#include <functional>
#include <string>
#include "NeuronException.h"

class Neuron {

public:
    static const std::function<float(float)> SIGMOID;
    static const std::function<float(float)> SIGMOID_DER;
    
    // identification parameters
    unsigned int id;
    
    // neuron parameters
    std::function<float(float)> af;     // the activation function
    std::function<float(float)> afD;    // the derivative of the activation function
private:
    static unsigned int idGenerator;    // the number used to generate the id of an entity
    bool isForwardPhase;
    float inputValue;                   // the input of the neuron
    float outputValue;                  // the output of the neuron
    float accumulatedError;             // the error used to evaluate delta
    float deltaValue;                   // the local weighted error used for backward propagation
    
    
public:
    Neuron(std::function<float(float)> activationFunciton);
    Neuron(std::function<float(float)> activationFunciton, std::function<float(float)> activationFuncitonDerivative);
    
    void    in(float value);
    void    in(float weight, float nwValue);
    float   out();
    void    err(float errorToAdd);
    float   errModifier();
    float   delta();
    
    void newRound();
    
private:
    // neuron methods
    void computeOutput();
    void computeDelta();
    // foo
    void init();
    
    std::function<float(float)> computeDerivative(std::function<float(float)> fun);
    
};


#endif // NEURON_H