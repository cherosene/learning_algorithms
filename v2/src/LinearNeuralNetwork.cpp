#include "LinearNeuralNetwork.h"
#include "LinearNeuralNetworkException.h"

LinearNeuralNetwork::LinearNeuralNetwork(float alpha, std::vector<unsigned int> descriptor, std::function<float(float)> af, std::function<float(float)> afD) : alpha(alpha) {
    if(descriptor.size() == 0) {
        throw LinearNeuralNetworkException(LinearNeuralNetworkException::DESCRIPTOR_EMPTY);
    }
    layerNumber = descriptor.size();
    
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
                axons[layer-1].push_back( Axon(INITIAL_WEIGHT,neurons[layer-1][sourcePos],neurons[layer][targetPos]) );
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
    
    // output layer
    for(size_t pos = 0; pos < neurons[layerNumber-1].size(); ++pos) {
        neurons[layerNumber-1][pos].err( errVector[pos] );
    }
    
    // other layers
    for(size_t layer = layerNumber; layer-- > 0 ; ) {
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].giveErr();
        }
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].update(alpha);
        }
    }
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