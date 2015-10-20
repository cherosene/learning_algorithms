#ifndef NEURONEXCEPTION_H
#define NEURONEXCEPTION_H

class NeuronException: public std::exception {
public:
    unsigned int id;

public:
    NeuronException(unsigned int idReceived) {
        id = idReceived;
    }
    virtual const char* what() const throw() {
        return "Neuron Exception detected";
    }
};


#endif // NEURONEXCEPTION_H