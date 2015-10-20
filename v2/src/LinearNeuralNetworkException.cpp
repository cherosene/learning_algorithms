#include "LinearNeuralNetworkException.h"


const char* const LinearNeuralNetworkException::Reason[3] = {
    "The descriptor is empty.",
    "Wrong input length.",
    "Wrong error vector length."
};


LinearNeuralNetworkException::LinearNeuralNetworkException(errcode errId) {
    err = errId;
}

const char* LinearNeuralNetworkException::what() const throw() {
    return Reason[err];
}