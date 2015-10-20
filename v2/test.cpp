#include "LinearNeuralNetwork.h"
#include <iostream>
#include <cmath>


int main() {
    
    std::vector<unsigned int> descriptor = { 1, 3, 1 };
    LinearNeuralNetwork nn = LinearNeuralNetwork( 0.5, descriptor, Neuron::SIGMOID, Neuron::SIGMOID_DER );
    
    try {
        std::cout << "A" << std::endl;
        float res = nn.out( {1} )[0];
        
        std::cout << "B" << std::endl;
        nn.learn( {5 - res} );
        
        std::cout << "C" << std::endl;
        std::cout << nn.debug() << std::endl;
        
        std::cout << "D" << std::endl;
        std::cout << nn.out( {1} )[0] << std::endl;
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
    return 0;
    
}