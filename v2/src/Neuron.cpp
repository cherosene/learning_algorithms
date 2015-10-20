#include "Neuron.h"

#include <iostream>
#include <cmath>


unsigned int Neuron::idGenerator = 0;

const std::function<float(float)> Neuron::SIGMOID      = [](float r){
    float res = 1 + exp(-r);
    return 1/res;
};

const std::function<float(float)> Neuron::SIGMOID_DER  = [](float r){
    float res = 2 * ( cosh(r) + 1 );
    return 1/res;
};



Neuron::Neuron(std::function<float(float)> activationFunciton) {
    Neuron(activationFunciton, computeDerivative(activationFunciton) );
}

Neuron::Neuron(std::function<float(float)> activationFunciton, std::function<float(float)> activationFuncitonDerivative) {
    id = idGenerator;
    idGenerator++;
    
    af = activationFunciton;
    afD = activationFuncitonDerivative;
    
    init();
}

void Neuron::in(float value) { inputValue += value; }

void Neuron::in(float weight, float nwValue) { inputValue += weight * nwValue; }

float Neuron::out() {
    if(isForwardPhase) { computeOutput(); }
    return outputValue;
}

void Neuron::err(float errorToAdd) {
    accumulatedError += errorToAdd;
}

float Neuron::errModifier() {
    return afD(inputValue);
}

float Neuron::delta() { return deltaValue; }


void Neuron::newRound() {
    computeDelta();
    
    inputValue          = 0;
    outputValue         = 0;
    accumulatedError    = 0;
    isForwardPhase      = true;
}


// neuron methods
void Neuron::computeOutput() {
    if(!isForwardPhase) {
        throw NeuronException(id);
    }
    outputValue = af(inputValue);
    isForwardPhase = false;
}

void Neuron::computeDelta() {
    if(isForwardPhase) {
        throw NeuronException(id);
    }
    
    deltaValue = afD(inputValue) * accumulatedError;
}


// foo
void Neuron::init() {
    isForwardPhase      = true;
    inputValue          = 0;
    outputValue         = 0;
    deltaValue          = 0;
    accumulatedError    = 0;
}

std::function<float(float)> Neuron::computeDerivative(std::function<float(float)> fun) {
    std::cout << "computeDerivative: Not implemented yet." << std::endl;
    exit(-1);
}