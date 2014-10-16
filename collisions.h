#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <stdbool.h>
#include <stdint.h>

#include "species.h"

void collision_with_food(struct species *subject);
void collision_vision(struct species *subject);
void collision_attack(struct species *subject);

#endif
