#include <fstream>
#include "LinearNeuralNetwork.h"
#include "LinearNeuralNetworkException.h"

#include <cstdlib>
#include <iostream>


const unsigned int MAX_LENGTH_NAME = 10;


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

// FIXME: this function expect a well formatted file; there are no controls!
LinearNeuralNetwork::LinearNeuralNetwork(const char *filename, std::function<float(float)> af, std::function<float(float)> afD) {
    std::ifstream inputFile;
    inputFile.open(filename);
    
    // load name
    char name[MAX_LENGTH_NAME];
    for(size_t letter = 0; letter < MAX_LENGTH_NAME; ++letter) {
        inputFile.read( name + letter, sizeof(char) );
    }
    std::cout << "Loading neural network: " << name << std::endl;
    
    // load alpha
    inputFile.read( (char *)&(alpha), sizeof(float) );
    
    // load topology and create neurons
    inputFile.read( (char *)&(layerNumber), sizeof(unsigned int) );
    for(size_t layer = 0; layer < layerNumber; ++layer) {
        neurons.push_back( std::vector<Neuron>() );
        unsigned int layerLength;
        inputFile.read( (char*)&(layerLength), sizeof(unsigned int) );
        for(size_t i = 0; i < layerLength; ++i) { neurons[layer].push_back( Neuron(af,afD) ); }
    }
    
    // create axons while loading weights
    for(size_t layer = 1; layer < layerNumber; ++layer) {
        axons.push_back( std::vector<Axon>() );
        for(size_t sourcePos = 0; sourcePos < neurons[layer-1].size(); ++sourcePos) {
            for(size_t targetPos = 0; targetPos < neurons[layer].size(); ++targetPos) {
                float weight;
                inputFile.read( (char*)&(weight), sizeof(float) );
                axons[layer-1].push_back( Axon(weight,neurons[layer-1][sourcePos],neurons[layer][targetPos]) );
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
    if( neurons[layerNumber-1].size() != errVector.size() ) { throw LinearNeuralNetworkException(LinearNeuralNetworkException::ERROR_LENGTH); }
    
    // output layer
    for(size_t pos = 0; pos < neurons[layerNumber-1].size(); ++pos) {
        neurons[layerNumber-1][pos].err( errVector[pos] );
        neurons[layerNumber-1][pos].learn();
        neurons[layerNumber-1][pos].newRound();
    }
    
    // other layers
    for(size_t layer = layerNumber-1; layer-- > 0 ; ) {
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].giveErr();
        }
        for(size_t pos = 0; pos < neurons[layer].size(); ++pos) {
            neurons[layer][pos].learn();
            neurons[layer][pos].newRound();
        }
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            axons[layer][pos].update(alpha);
        }
    }
    
}

void LinearNeuralNetwork::save(const char* filename, const char* name) {
    unsigned int nameLength = strlen(name);
    if( nameLength > MAX_LENGTH_NAME ) { throw LinearNeuralNetworkException(LinearNeuralNetworkException::SAVE_NAME_TOO_LONG); }
    
    std::ofstream outputFile;
    outputFile.open(filename);
    
    // saving name of the neural network
    outputFile.write( name, sizeof(char) * nameLength );
    if( nameLength != MAX_LENGTH_NAME ) {
        char zero = 0;
        for(size_t i = 0; i < MAX_LENGTH_NAME - nameLength; ++i) { outputFile.write( &zero, sizeof(char) ); }
    }
    
    // saving parameter alpha
    outputFile.write( (char *)&(alpha), sizeof(float) );
    
    // saving topology
    outputFile.write( (char *)&(layerNumber), sizeof(unsigned int) );
    for(size_t layer = 0; layer < layerNumber; ++layer) {
        unsigned int l = neurons[layer].size();
        outputFile.write( (char *)&l, sizeof(size_t) );
    }
    
    // saving axons weights
    for(size_t layer = 0; layer < layerNumber-1; ++layer) {
        for(size_t pos = 0; pos < axons[layer].size(); ++pos) {
            outputFile.write( (char *)&(axons[layer][pos].weight), sizeof(float) );
        }
    }
}


