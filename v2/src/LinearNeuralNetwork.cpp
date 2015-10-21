#include "LinearNeuralNetwork.h"
#include "LinearNeuralNetworkException.h"

#include <cstdlib>
#include <iostream>


LinearNeuralNetwork::LinearNeuralNetwork(float alpha, std::vector<unsigned int> descriptor, std::function<float(float)> af, std::function<float(float)> afD) : alpha(alpha) {
    if(descriptor.size() == 0) {
        throw LinearNeuralNetworkException(LinearNeuralNetworkException::DESCRIPTOR_EMPTY);
    }
    layerNumber = descriptor.size();
    
    // random initialization
    srand (static_cast <unsigned> (clock()));
    
    // input layer - layer 0
    neurons.push_back( std::vector<Neuron>() );
    for(size_t pos = 0; pos < descriptor[0]; ++pos) {
        neurons[0].push_back( Neuron(af,afD) );
    }
    
    // other layers
    for(size_t layer = 1; layer < layerNumber; ++layer) {
        // add neurons
        neurons.push_back( std::vector<Neuron>() );
        for(size_t pos = 0; pos < descriptor[layer]; ++pos) {
            neurons[layer].push_back( Neuron(af,afD) );
        }
        
        // add axons
        axons.push_back( std::vector<Axon>() );
        for(size_t sourcePos = 0; sourcePos < neurons[layer-1].size(); ++sourcePos) {
            for(size_t targetPos = 0; targetPos < neurons[layer].size(); ++targetPos) {
                float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/MAX_INITIAL_WEIGHT));
                axons[layer-1].push_back( Axon(r,neurons[layer-1][sourcePos],neurons[layer][targetPos]) );
            }
        }
        
    }
}


std::vector<float> LinearNeuralNetwork::out(std::vector<float> inVector) {
    if( neurons[0].size() != inVector.size() ) { throw LinearNeuralNetworkException(LinearNeuralNetworkException::INPUT_LENGTH); }
    
    // input layer
    for(size_t pos = 0; pos < neurons[0].size(); ++pos) {
        neurons[0][pos].in(inVector[pos]);
    }
    
    // other layers
    for(size_t layer = 0; layer < layerNumber-1; ++layer) {
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].giveOut();
        }
    }
    
    // create output vector
    std::vector<float> result = std::vector<float>();
    for(size_t pos = 0; pos < neurons[layerNumber-1].size(); ++pos) {
        result.push_back( neurons[layerNumber-1][pos].out() );
    }
    
    return result;
}

void LinearNeuralNetwork::learn(std::vector<float> errVector) {
    // TODO: gives segfault
    if( neurons[layerNumber-1].size() != errVector.size() ) { throw LinearNeuralNetworkException(LinearNeuralNetworkException::ERROR_LENGTH); }

    // std::cout << "DEBUG MODE" << std::endl;
//     std::cout << debug() << std::endl;
    
    // output layer
    for(size_t pos = 0; pos < neurons[layerNumber-1].size(); ++pos) {
        neurons[layerNumber-1][pos].err( errVector[pos] );
        neurons[layerNumber-1][pos].learn();
        // std::cout << neurons[layerNumber-1][pos].delta() << std::endl;
//         std::cout << "input " << neurons[layerNumber-1][pos].inputValue << std::endl;
//         std::cout << "accError " << neurons[layerNumber-1][pos].accumulatedError << std::endl;
        neurons[layerNumber-1][pos].newRound();
    }
    
    // other layers
    for(size_t layer = layerNumber-1; layer-- > 0 ; ) {
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].giveErr();
        }
        for(size_t pos = 0; pos < neurons[layer].size(); ++pos) {
            neurons[layer][pos].learn();
            // std::cout << neurons[layer][pos].delta() << std::endl;
//             std::cout << "input " << neurons[layer][pos].inputValue << " " << neurons[layer][pos].afD( neurons[layer][pos].inputValue ) << " " << Neuron::SIGMOID_DER( neurons[layer][pos].inputValue ) << std::endl;
//             std::cout << "accError " << neurons[layer][pos].accumulatedError << std::endl;
            neurons[layer][pos].newRound();
        }
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].update(alpha);
        }
    }
    
    // std::cout << debug() << std::endl;
}





std::string LinearNeuralNetwork::debug() {
    std::string result = std::to_string( neurons[0].size() ) + "\n";
    
    for(size_t layer = 0; layer < layerNumber-1; ++layer) {
        result += "[ ";
        
        for(size_t i = 0; i < axons[layer].size(); ++i) {
            result += "(" + std::to_string(i) + "," + std::to_string(axons[layer][i].weight) + ") ";
        }
        result += "]\n" + std::to_string( neurons[layer+1].size() ) + "\n";
    }
    
    return result;
}