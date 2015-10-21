#include "LinearNeuralNetwork.h"
#include <iostream>
#include <cmath>
#include <ctime>


const float PRECISION = 0.0001;

const std::function<float(float)> sgn = [](float r){
    if(r > 0) { return 1;   }
    if(r < 0) { return -1;  }
    return 0;
};

int main() {
    
    std::vector<unsigned int> descriptor = { 1, 3, 5, 3, 1 };
    LinearNeuralNetwork nn = LinearNeuralNetwork( 0.5, descriptor, Neuron::SIGMOID, Neuron::SIGMOID_DER );
    
    srand (static_cast <unsigned> (clock()));
    rand(); rand(); rand();
    float randValue = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
    
    try {
        unsigned int counter = 0;
        float res = nn.out( {1} )[0];
        std::cout << res << " (error: " << res - randValue << ")" << std::endl;
        nn.learn( {sgn(randValue - res)} );

        while(std::abs(randValue - res) > PRECISION) {
            counter++;
            res = nn.out( {1} )[0];
            std::cout << res << " (error: " << res - randValue << ")" << std::endl;
            nn.learn( {sgn(randValue - res)} );
        }
        std::cout << "Number of iterations: " << counter << std::endl;
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    
    return 0;
    
}