#include "Axon.h"

Axon::Axon(float initialWeight, Neuron& s, Neuron& t) : source(&s), target(&t) {
    weight = initialWeight;
}


void Axon::giveOut() {
    target->in( weight * source->out() );
}

void Axon::giveErr() {
    source->err( source->errModifier() * weight * target->delta() );
}


void Axon::update(float alpha) {
    weight += alpha * source->out() * target->delta();
}