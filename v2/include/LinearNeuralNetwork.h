#ifndef LINEARNEURALNETWORK_H
#define LINEARNEURALNETWORK_H

#include <vector>
#include "Axon.h"

class LinearNeuralNetwork {
    
public:
    constexpr static float MAX_INITIAL_WEIGHT = 1;
    
public:
    // TODO: I don't like thoose names; I have to change them
    std::vector<std::vector<Neuron>>    neurons;
    std::vector<std::vector<Axon>>      axons;
    std::vector<Axon>                   bias;
    
    unsigned int layerNumber;
    float alpha;
    
public:
    LinearNeuralNetwork(float alpha, std::vector<unsigned int> descriptor, std::function<float(float)> af, std::function<float(float)> afD);
    LinearNeuralNetwork(const char *filename, std::function<float(float)> af, std::function<float(float)> afD);
    
    std::vector<float> out(std::vector<float> inVector);
    void learn(std::vector<float> errVector);
    
    void save(const char* filename, const char* name);
    void debug();
    
};


#endif // LINEARNEURALNETWORK_H