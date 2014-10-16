#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "species.h"
#include "utils.h"
#include "output_functions.h"
#include "input_functions.h"
#include "graphic.h"
#include "world.h"
#include "collisions.h"
#include "graphic.h"

uint8_t subject_number = 0;

struct species *create_subject_1()
{
    subject_number++;
    struct species *subject = (struct species*) malloc(sizeof(struct species));
    // base data of the subject
    subject->pos_x = rand()%WORLD_WIDTH;
    subject->pos_y = rand()%WORLD_HEIGHT;
    subject->orient = rand();
    subject->red_color = 0;
    subject->green_color = 0;
    subject->blue_color = UINT8_T_MAX;
    subject->input_size = BASE_SIZE;
    subject->input_energy = UINT8_T_HALF_MAX;
    subject->input_captor_red = 0;
    subject->input_captor_green = 0;
    subject->input_captor_blue = 0;
    subject->input_captor_feel = 0;
    subject->output_attack = 0;
    subject->output_forward = 0;
    subject->output_turn_right = 0;
    subject->output_turn_left = 0;
    subject->output_reproduce = 0;
    // neural data of the subject
    struct neural_data *neural = &(subject->neural);
    const int layer_nb = 2;
    const int layer1_nb = 4;
    const int layer2_nb = 5;
    neural->layer_nb = layer_nb;
    neural->neuron_layers = (struct neuron_layer*) malloc(layer_nb*sizeof(struct neuron_layer));

    neural->neuron_layers[0].layer_size = layer1_nb;
    struct neuron *neurons = (struct neuron*)malloc(layer1_nb*sizeof(struct neuron));
    uint8_t i;
    for (i=0; i<layer1_nb; i++)
    {
        neurons[i].output = 0;
        neurons[i].input_number = layer1_nb;

        int16_t **inputs = (int16_t**) malloc(layer1_nb*sizeof(int16_t*));
        inputs[0] = &(subject->input_captor_red);
        inputs[1] = &(subject->input_captor_green);
        inputs[2] = &(subject->input_captor_blue);
        inputs[3] = &(subject->input_captor_feel);
        neurons[i].inputs = inputs;

        int16_t *weights = (int16_t*) malloc((layer1_nb+1)*sizeof(int16_t));
        weights[0] = 0;
        weights[1] = rand()%2 - 1;
        weights[2] = rand()%2 - 1;
        weights[3] = rand()%2 - 1;
        weights[4] = rand()%2 - 1;
        neurons[i].weights = weights;

        neurons[i].output_function = Identity_function; // Une fonction pour le calcul de la sortie !!! A CHANGER EN SIGMOIDE
    }
    neural->neuron_layers[0].neurons = neurons;

    neural->neuron_layers[1].layer_size = layer2_nb;
    neurons = (struct neuron*) malloc(layer2_nb*sizeof(struct neuron));
    for (i=0; i<layer2_nb; i++)
    {
        neurons[i].output = 0;
        neurons[i].input_number = layer1_nb;

        int16_t **inputs = (int16_t**) malloc(layer1_nb*sizeof(int16_t*));
        inputs[0] = &(neural->neuron_layers[0].neurons[0].output);
        inputs[1] = &(neural->neuron_layers[0].neurons[1].output);
        inputs[2] = &(neural->neuron_layers[0].neurons[2].output);
        inputs[3] = &(neural->neuron_layers[0].neurons[3].output);
        neurons[i].inputs = inputs;

        int16_t *weights = (int16_t*) malloc((layer1_nb+1)*sizeof(int16_t));
        weights[0] = rand()%2 - 1;
        weights[1] = rand()%2 - 1;
        weights[2] = rand()%2 - 1;
        weights[3] = rand()%2 - 1;
        weights[4] = rand()%2 - 1;
        neurons[i].weights = weights;

        neurons[i].output_function = Identity_function; // Une fonction pour le calcul de la sortie
    }
    /* section "pas trop con" */
    neurons[0].weights[0] = -127; // j'évite d'attaquer tout le temps comme un mongole
    neurons[1].weights[0] = 127; // j'avance
    /* fin section */
    neural->neuron_layers[1].neurons = neurons;
    return subject;
}

struct species *copy_species_1(struct species *subject)
{
    struct species *new_subject = (struct species*) malloc(sizeof(struct species));
    *new_subject = *subject;
    struct neuron_layer *neuron_layers = (struct neuron_layer*) malloc(subject->neural.layer_nb*sizeof(struct neuron_layer));

    neuron_layers[0].layer_size = subject->neural.neuron_layers[0].layer_size;
    struct neuron *neurons = (struct neuron*) malloc(neuron_layers[0].layer_size*sizeof(struct neuron));
    uint8_t i;
    for (i=0; i<neuron_layers[0].layer_size; i++)
    {
        neurons[i].output = 0;
        neurons[i].input_number = neuron_layers[0].layer_size;

        int16_t **inputs = (int16_t**) malloc(neuron_layers[0].layer_size*sizeof(int16_t*));
        inputs[0] = &(new_subject->input_energy);
        inputs[1] = &(new_subject->input_captor_red);
        inputs[2] = &(new_subject->input_captor_green);
        inputs[3] = &(new_subject->input_captor_blue);
        inputs[4] = &(new_subject->input_captor_feel);
        neurons[i].inputs = inputs;

        int16_t *weights = (int16_t*) malloc((neuron_layers[0].layer_size+1)*sizeof(int16_t));
        weights[0] = subject->neural.neuron_layers[0].neurons[i].weights[0];
        weights[1] = subject->neural.neuron_layers[0].neurons[i].weights[1];
        weights[2] = subject->neural.neuron_layers[0].neurons[i].weights[2];
        weights[3] = subject->neural.neuron_layers[0].neurons[i].weights[3];
        weights[4] = subject->neural.neuron_layers[0].neurons[i].weights[4];
        weights[5] = subject->neural.neuron_layers[0].neurons[i].weights[5];
        neurons[i].weights = weights;

        neurons[i].output_function = Identity_function;
    }
    neuron_layers[0].neurons = neurons;

    neuron_layers[1].layer_size = subject->neural.neuron_layers[1].layer_size;
    neurons = (struct neuron*) malloc(neuron_layers[1].layer_size*sizeof(struct neuron));
    for (i=0; i<neuron_layers[1].layer_size; i++)
    {
        neurons[i].output = 0;
        neurons[i].input_number = neuron_layers[1].layer_size;

        int16_t **inputs = (int16_t**) malloc(neuron_layers[1].layer_size*sizeof(int16_t*));
        inputs[0] = &(new_subject->neural.neuron_layers[0].neurons[0].output);
        inputs[1] = &(new_subject->neural.neuron_layers[0].neurons[1].output);
        inputs[2] = &(new_subject->neural.neuron_layers[0].neurons[2].output);
        inputs[3] = &(new_subject->neural.neuron_layers[0].neurons[3].output);
        inputs[4] = &(new_subject->neural.neuron_layers[0].neurons[4].output);
        neurons[i].inputs = inputs;

        int16_t *weights = (int16_t*) malloc((neuron_layers[1].layer_size+1)*sizeof(int16_t));
        weights[0] = subject->neural.neuron_layers[1].neurons[i].weights[0];
        weights[1] = subject->neural.neuron_layers[1].neurons[i].weights[1];
        weights[2] = subject->neural.neuron_layers[1].neurons[i].weights[2];
        weights[3] = subject->neural.neuron_layers[1].neurons[i].weights[3];
        weights[4] = subject->neural.neuron_layers[1].neurons[i].weights[4];
        weights[5] = subject->neural.neuron_layers[1].neurons[i].weights[5];
        neurons[i].weights = weights;

        neurons[i].output_function = Identity_function; // Une fonction pour le calcul de la sortie
    }
    neuron_layers[1].neurons = neurons;
    new_subject->neural.neuron_layers = neuron_layers;
    return new_subject;
}


void reproduce(struct species *subject)
{
    if ( subject_number < MAX_SPECIES && subject->red_color < UINT16_T_MAX ) {
        subject_number++;
        struct species_list *species = *world_species_return();
        while (species != NULL && (species)->subject != subject)
        {
            species = species->next;
        }
        if (species != NULL) {
            subject->input_energy = subject->input_energy >> 1;
            struct species_list *new_species = (struct species_list*) malloc(sizeof(struct species_list));
            new_species->subject = copy_species_1(species->subject);

            new_species->next = species->next;
            species->next = new_species;
            new_species->subject->orient += UINT8_T_QUARTER_MAX;

            mutate_neural(new_species->subject);
        }
    }
}


void update_subject_1(struct species *subject)
{
    if (world_timestamp_return() % 8 == 0)
        subject->input_energy--;
    if (subject->input_energy > 0) {
        captor_function_vision(subject);

        uint8_t j;
        uint8_t k;
        for (k=0; k<subject->neural.layer_nb; k++)
        {
            for (j=0; j<subject->neural.neuron_layers[k].layer_size; j++)
            {
                uint8_t input_number = subject->neural.neuron_layers[k].neurons[j].input_number;
                int16_t **inputs = subject->neural.neuron_layers[k].neurons[j].inputs;
                int16_t *weights = subject->neural.neuron_layers[k].neurons[j].weights;
                subject->neural.neuron_layers[k].neurons[j].output =
                        subject->neural.neuron_layers[k].neurons[j].output_function(input_number, inputs, weights);
            }
        }

        collision_with_food(subject);
        //printf("forward : %d\n", subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[1].output);


        subject->output_attack = subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[0].output > 0;
        subject->output_forward = subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[1].output > 0;
        subject->output_turn_right = subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[2].output > 0;
        subject->output_turn_left = subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[3].output > 0;
        subject->output_reproduce = subject->neural.neuron_layers[subject->neural.layer_nb - 1].neurons[4].output > 0;
        if (subject->output_attack)
            attack(subject);
        if (subject->output_forward)
            go_forward(subject);
        if (subject->output_turn_right)
            turn_right(subject);
        if (subject->output_turn_left)
            turn_left(subject);
        if (subject->input_energy > UINT8_T_MAX)
            reproduce(subject);
    }  else {
        world_become_meat(subject);
    }
}

void display_subject_1(struct species *subject, uint8_t display_type)
{
    graphic_draw_circle_colored(subject->pos_x, subject->pos_y, subject->input_size, subject->red_color, subject->green_color, subject->blue_color);
    uint32_t x_eye = subject->pos_x + subject->input_size*cos(subject->orient/TWOPI);
    uint32_t y_eye = subject->pos_y + subject->input_size*sin(subject->orient/TWOPI);
    if (subject->output_attack) {
        uint32_t x1 = subject->pos_x + subject->input_size*cos(subject->orient/TWOPI);
        uint32_t y1 = subject->pos_y + subject->input_size*sin(subject->orient/TWOPI);
        uint32_t x2 = subject->pos_x + 10*subject->input_size*cos((subject->orient)/TWOPI - PI_ON_SIXTEEN);
        uint32_t y2 = subject->pos_y + 10*subject->input_size*sin((subject->orient)/TWOPI - PI_ON_SIXTEEN);
        uint32_t x3 = subject->pos_x + 10*subject->input_size*cos((subject->orient)/TWOPI + PI_ON_SIXTEEN);
        uint32_t y3 = subject->pos_y + 10*subject->input_size*sin((subject->orient)/TWOPI + PI_ON_SIXTEEN);
        graphic_draw_triangle_colored(x1, y1, x2, y2, x3, y3, 0, 0, 0);
    }

    graphic_draw_circle_colored(x_eye, y_eye, subject->input_size >> 2, 255, 255, 255);


    if (display_type > 0) {
    printf("(%d, %d) ,%d\n", subject->pos_x, subject->pos_y, subject->orient);
        if (display_type > 1) {
            printf("energy :%d\n", subject->input_energy);
            printf("captor red :%d\n", subject->input_captor_red);
            printf("captor green :%d\n", subject->input_captor_green);
            printf("captor blue :%d\n", subject->input_captor_blue);
            printf("captor feel :%d\n", subject->input_captor_feel);
            printf("neural data :\n");
            uint8_t i;
            uint8_t j;
            uint8_t k;
            for (k=0; k<subject->neural.layer_nb; k++)
            {
                printf("    layer %d\n", k);
                for (j=0; j<subject->neural.neuron_layers[k].layer_size; j++)
                {
                    printf("        neuron %d\n", j);
                    printf("            input number :%d\n", subject->neural.neuron_layers[k].neurons[j].input_number);
                    for (i=0; i<subject->neural.neuron_layers[k].neurons->input_number; i++ )
                    {
                        printf("                input %d : %d\n", i, *(subject->neural.neuron_layers[k].neurons[j].inputs[i]));
                        printf("                weight %d : %d\n", i, subject->neural.neuron_layers[k].neurons[j].weights[i]);
                    }
                    printf("output :%d\n", subject->neural.neuron_layers[k].neurons[j].output);
                    printf("\n");
                }
            }
        }
    }
}


void release_species(struct species *subject)
{
    int i,j;
    for (i=1; i<subject->neural.layer_nb; i++)
    {
        for (j=0; j<subject->neural.neuron_layers[i].layer_size; j++)
        {
            free(subject->neural.neuron_layers[i].neurons[j].inputs);
            free(subject->neural.neuron_layers[i].neurons[j].weights);
        }
        free(subject->neural.neuron_layers[i].neurons);
    }
    free(subject->neural.neuron_layers);
    free(subject);
    subject_number--;
    if (subject_number == 4 && graphic_is_open()) {
        (*world_species_return())->subject->input_energy = UINT8_T_MAX + 1;
        (*world_species_return())->next->subject->input_energy=UINT8_T_MAX + 1;
        (*world_species_return())->next->next->subject->input_energy=UINT8_T_MAX + 1;
        (*world_species_return())->next->next->next->subject->input_energy=UINT8_T_MAX + 1;
    }
}
