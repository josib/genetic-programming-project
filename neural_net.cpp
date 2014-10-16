#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "species.h"
#include "neural_net.h"
#include "utils.h"

int16_t sum_function(uint8_t input_number, int16_t **inputs, int16_t *weights)
{
    int32_t sum = weights[0];
    int32_t weights_sum = 0;
    uint8_t i;
    for (i = 0; i<input_number; i++)
    {
        sum += (*(inputs[i]))*weights[i+1];
        weights_sum += abs(weights[i+1]);
    }
    if (weights_sum != 0)
        sum /= weights_sum;
    return sum;
}

int16_t Identity_function(uint8_t input_number, int16_t **inputs, int16_t *weights)
{
    const uint16_t sum = sum_function(input_number, inputs, weights);
    return sum;
}


int16_t Sigmoid_function(uint8_t input_number, int16_t **inputs, int16_t *weights)
{
    const uint16_t sum = sum_function(input_number, inputs, weights);
    return UINT16_T_MAX/(1+exp(-sum));
}

void mutate_neural(struct species *subject)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    for (k=0; k<subject->neural.layer_nb; k++)
    {
        for (j=0; j<subject->neural.neuron_layers[k].layer_size; j++)
        {
            for (i=0; i<subject->neural.neuron_layers[k].neurons->input_number; i++ )
            {
                const uint8_t mutation = rand()%10;
                if (mutation == 0)
                    subject->neural.neuron_layers[k].neurons[j].weights[i] = (subject->neural.neuron_layers[k].neurons[j].weights[i] + 32);
                else if (mutation == 1)
                    subject->neural.neuron_layers[k].neurons[j].weights[i] = (subject->neural.neuron_layers[k].neurons[j].weights[i] - 32);
            }
        }
    }
}
