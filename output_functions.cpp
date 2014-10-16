#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <stdio.h>

#include "world.h"
#include "output_functions.h"
#include "utils.h"
#include "neural_net.h"
#include "collisions.h"

void go_forward(struct species *subject)
{
    subject->pos_x = (uint32_t)(subject->pos_x + 4*cos(subject->orient/TWOPI) + WORLD_WIDTH) % WORLD_WIDTH;
    subject->pos_y = (uint32_t)(subject->pos_y + 4*sin(subject->orient/TWOPI) + WORLD_HEIGHT) % WORLD_HEIGHT;
}

void turn_right(struct species *subject)
{
    subject->orient -= 1;
}

void turn_left(struct species *subject)
{
    subject->orient += 1;
}

void attack(struct species *subject)
{
    subject->input_energy -= 4;
    collision_attack(subject);
}

