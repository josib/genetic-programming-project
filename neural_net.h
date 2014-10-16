#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include <stdint.h>

struct neuron {
    int16_t output; // une sortie
    uint8_t input_number; // un nombre d'entrées
    int16_t **inputs; // un tableau de pointeurs pour les entrées
    int16_t *weights; // des poids pour pondérer les entrées
    int16_t (*output_function)(uint8_t, int16_t**, int16_t*); // Une fonction pour le calcul de la sortie
};

int16_t Identity_function(uint8_t input_number, int16_t **inputs, int16_t *weights);
int16_t Sigmoid_function(uint8_t input_number, int16_t **inputs, int16_t *weights);
void mutate_neural(struct species *subject);

struct neuron_layer {
    uint8_t layer_size;
    struct neuron *neurons;
};

struct neural_data {
    uint8_t layer_nb;
    struct neuron_layer *neuron_layers;
};

#endif
