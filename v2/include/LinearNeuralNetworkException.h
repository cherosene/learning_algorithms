#ifndef LINEARNEURALNETWORKEXCEPTION_H
#define LINEARNEURALNETWORKEXCEPTION_H

#include <exception>

class LinearNeuralNetworkException: public std::exception {

public:
	enum errcode {
        DESCRIPTOR_EMPTY,
        INPUT_LENGTH,
        ERROR_LENGTH,
        SAVE_NAME_TOO_LONG
    };
    
    errcode err;
    
public:
    static const char* const Reason[];
    

public:
    LinearNeuralNetworkException(errcode errId);
    const char* what() const throw();
    
};


#endif // LINEARNEURALNETWORKEXCEPTION_H