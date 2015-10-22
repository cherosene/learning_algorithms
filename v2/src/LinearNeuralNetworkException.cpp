#include "LinearNeuralNetworkException.h"


const char* const LinearNeuralNetworkException::Reason[4] = {
    "The descriptor is empty.",
    "Wrong input length.",
    "Wrong error vector length.",
    "The name used is too long."
};


LinearNeuralNetworkException::LinearNeuralNetworkException(errcode errId) {
    err = errId;
}

const char* LinearNeuralNetworkException::what() const throw() {
    return Reason[err];
}