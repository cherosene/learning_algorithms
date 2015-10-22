#include <iostream>
#include "LinearNeuralNetwork.h"
#include "sdlUtility.h"
#include "Entity.h"



const std::function<float(float)> sgn = [](float r){
    if(r > 0) { return 1;   }
    if(r < 0) { return -1;  }
    return 0;
};

int main() {
    
    std::vector<unsigned int> descriptor = { 1, 3 ,5 ,7, 4, 2 };
    
    LinearNeuralNetwork nn = LinearNeuralNetwork( 0.5, descriptor, Neuron::SIGMOID, Neuron::SIGMOID_DER );
    
    nn.debug();
    
    nn.save( "prova.nn", "ciccio" );
    
    LinearNeuralNetwork nn2 = LinearNeuralNetwork( "prova.nn", Neuron::SIGMOID, Neuron::SIGMOID_DER );
    
    nn2.debug();
    
    
    return 0;
    
}