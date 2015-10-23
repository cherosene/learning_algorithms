#include "Axon.h"

Axon::Axon(float initialWeight, Neuron& s, Neuron& t) : isBias(false), source(&s), target(&t) {
    weight = initialWeight;
}

Axon::Axon(float initialWeight, Neuron& t) : isBias(true), source(NULL), target(&t) {
    weight = initialWeight;
}


void Axon::giveOut() {
    if(isBias)  { target->in( - weight ); }
    else        { target->in( weight * source->out() ); }
}

void Axon::giveErr() {
    if(!isBias) { source->err( source->errModifier() * weight * target->delta() ); }
}


void Axon::update(float alpha) {
    if(isBias)  { weight -= alpha * target->delta(); }
    else        { weight += alpha * source->out() * target->delta(); }
}