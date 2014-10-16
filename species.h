#ifndef SPECIES_H
#define SPECIES_H

#include <stdint.h>
#include <stdbool.h>

#include "neural_net.h"

struct species {
    struct neural_data neural;
    uint32_t pos_x;
    uint32_t pos_y;
    uint8_t orient;
    uint8_t red_color;
    uint8_t green_color;
    uint8_t blue_color;
    int16_t input_size; // pas encore géré
    int16_t input_energy; // pas encore géré
    int16_t input_captor_red;
    int16_t input_captor_green;
    int16_t input_captor_blue;
    int16_t input_captor_feel;
    bool output_attack;
    bool output_forward;
    bool output_turn_right;
    bool output_turn_left;
    bool output_reproduce;
};

struct species *create_subject_1();
void reproduce(struct species *subject);
void update_subject_1(struct species *subject);
void display_subject_1(struct species *subject, uint8_t display_type); // 0 = base display (graphic) 1 = console basic display, 2 = console extended display

void release_species(struct species *subject);

#endif
