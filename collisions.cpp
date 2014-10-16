#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "collisions.h"
#include "world.h"
#include "utils.h"


const uint8_t food_gain = 64;

bool collide(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t radius_sum)
{
    if ( (abs(x1 - x2) < radius_sum) && (abs(y1 - y2) < radius_sum) ) {
        const int32_t distance = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
        if (distance < radius_sum*radius_sum)
            return true;
        else
            return false;
    } else {
        return false;
    }
}

void collision_with_food(struct species *subject)
{
    const uint32_t pos_x = subject->pos_x;
    const uint32_t pos_y = subject->pos_y;
    const uint8_t radius = subject->input_size;

    uint16_t gain = 0;

    struct food_list *food = world_food_return();
    while (food != NULL)
    {
        if (food->timer == 0 && collide(food->pos_x, food->pos_y, pos_x, pos_y, radius + BASE_SIZE) ) {
            food->pos_x = rand() % WORLD_WIDTH;
            food->pos_y = rand() % WORLD_HEIGHT;
            food->timer = FOOD_TIMER;
            gain += food_gain + (subject->blue_color >> 4);
            if (subject->red_color != 0) {
                subject->red_color -= 4;
                subject->blue_color += 4;
            }
            subject->input_size = BASE_SIZE + (subject->red_color >> 5) ;
        }
        food = food->next;
    }
    struct meat_list *meat = world_meat_return();
    struct meat_list *next_meat;
    while (meat != NULL)
    {
        next_meat = meat->next;
        if (collide(meat->pos_x, meat->pos_y, pos_x, pos_y, radius + BASE_SIZE)) {
            free_meat(meat);
            gain += 4*food_gain + (subject->red_color >> 2) - 1;
            if (subject->blue_color != 0) {
                subject->red_color += 16;
                subject->blue_color -= 16;
            }
            subject->input_size = BASE_SIZE + (subject->red_color >> 6);
        }
        meat = next_meat;
    }

    if (gain + subject->input_energy > UINT16_T_HALF_MAX)
        subject->input_energy = UINT16_T_HALF_MAX;
    else
        subject->input_energy += gain;
}

void collision_vision(struct species *subject)
{
    uint8_t occurences = 0;
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint32_t feel = 0;

    const double v_x = cos(subject->orient/TWOPI);
    const double v_y = sin(subject->orient/TWOPI);
    const double angle_source = atan2(v_y, v_x);

    struct food_list *food = world_food_return();
    while (food != NULL)
    {
        if (food->timer == 0 && collide(food->pos_x, food->pos_y, subject->pos_x, subject->pos_y, 10*subject->input_size) )
        {
            const int32_t distance = sqrt((food->pos_x - subject->pos_x)*(food->pos_x - subject->pos_x)
                                + (food->pos_y - subject->pos_y)*(food->pos_y - subject->pos_y));

            if (collide(food->pos_x, food->pos_y, subject->pos_x, subject->pos_y, 5*subject->input_size)
                && (5*subject->input_size - distance)*UINT8_T_MAX/(5*subject->input_size) > feel)
                feel = (5*subject->input_size - distance)*UINT8_T_HALF_MAX/(5*subject->input_size);

            const double y_diff = (int32_t)food->pos_y - (int32_t)subject->pos_y;
            const double x_diff = (int32_t)food->pos_x - (int32_t)subject->pos_x;
            const double angle_food = atan2(y_diff, x_diff);
            if (fabs(angle_source - angle_food) < PI_ON_SIXTEEN) {
                green += UINT8_T_HALF_MAX;
                occurences ++;
            }
        }
    food = food->next;
    }

    struct meat_list *meat = world_meat_return();
    while (meat != NULL)
    {
        const int32_t distance = sqrt((meat->pos_x - subject->pos_x)*(meat->pos_x - subject->pos_x)
                                       + (meat->pos_y - subject->pos_y)*(meat->pos_y - subject->pos_y));
        if (distance < 10*subject->input_size)
        {
            if ((10*subject->input_size - distance)*UINT8_T_MAX/(10*subject->input_size) > feel)
                feel = (10*subject->input_size - distance)*UINT8_T_HALF_MAX/(10*subject->input_size);

            const double y_diff = (int32_t)meat->pos_y - (int32_t)subject->pos_y;
            const double x_diff = (int32_t)meat->pos_x - (int32_t)subject->pos_x;
            const double angle_food = atan2(y_diff, x_diff);
            if (fabs(angle_source - angle_food) < PI_ON_SIXTEEN) {
                red += UINT8_T_HALF_MAX;
                occurences ++;
            }
        }
    meat = meat->next;
    }

    struct species_list *species = *world_species_return();
    while (species != NULL)
    {
        if (species->subject != subject) {
            const int32_t distance = sqrt((species->subject->pos_x - subject->pos_x)*(species->subject->pos_x - subject->pos_x)
                                           + (species->subject->pos_y - subject->pos_y)*(species->subject->pos_y - subject->pos_y));
            if (distance < 10*subject->input_size)
            {
                if ((10*subject->input_size - distance)*UINT8_T_MAX/(10*subject->input_size) > feel)
                    feel = (10*subject->input_size - distance)*UINT8_T_HALF_MAX/(10*subject->input_size);

                const double y_diff = (int32_t)species->subject->pos_y - (int32_t)subject->pos_y;
                const double x_diff = (int32_t)species->subject->pos_x - (int32_t)subject->pos_x;
                const double angle_food = atan2(y_diff, x_diff);
                if (fabs(angle_source - angle_food) < PI_ON_SIXTEEN) {
                    red += species->subject->red_color >> 1;
                    green += species->subject->green_color >> 1;
                    blue += species->subject->blue_color >> 1;
                    occurences ++;
                }
            }
        }
        species = species->next;
    }


    subject->input_captor_feel = feel;
    if (occurences != 0) {
        subject->input_captor_red = red/occurences;
        subject->input_captor_green = green/occurences;
        subject->input_captor_blue = blue/occurences;
    } else {
        subject->input_captor_red = 0;
        subject->input_captor_green = 0;
        subject->input_captor_blue = 0;
    }
}

void collision_attack(struct species *subject)
{
    const double v_x = cos(subject->orient/TWOPI);
    const double v_y = sin(subject->orient/TWOPI);
    const double angle_source = atan2(v_y, v_x);

    struct species_list *species = *world_species_return();
    while (species != NULL)
    {
        if (species->subject != subject) {
            const int32_t distance = sqrt((species->subject->pos_x - subject->pos_x)*(species->subject->pos_x - subject->pos_x)
                                           + (species->subject->pos_y - subject->pos_y)*(species->subject->pos_y - subject->pos_y));
            if (distance < 10*subject->input_size)
            {
                const double y_diff = (int32_t)species->subject->pos_y - (int32_t)subject->pos_y;
                const double x_diff = (int32_t)species->subject->pos_x - (int32_t)subject->pos_x;
                const double angle_food = atan2(y_diff, x_diff);
                if (fabs(angle_source - angle_food) < PI_ON_SIXTEEN) {
                    world_become_meat(species->subject);
                }
            }
        }
    species = species->next;
    }
}
