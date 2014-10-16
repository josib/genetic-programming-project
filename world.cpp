#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "world.h"
#include "species.h"
#include "utils.h"
#include "graphic.h"
#include "output_functions.h"

struct species_list *species;
struct species_list *becoming_meat = NULL;
struct food_list *food;
struct meat_list *meat;
uint32_t world_timestamp;

void world_init(uint8_t species_number, uint8_t food_number)
{
    srand(time(NULL));
    world_timestamp = 0;
    uint8_t i;
    species = NULL;
    for (i=0; i<species_number; i++)
    {
        struct species_list *new_specimen = (struct species_list*) malloc(sizeof(struct species_list));
        new_specimen->subject = create_subject_1();
        new_specimen->next = species;
        species = new_specimen;
    }

    food = NULL;
    for (i=0; i<food_number; i++)
    {
    	struct food_list *new_food = (struct food_list*) malloc(sizeof(struct food_list));
    	new_food->pos_x = rand()%WORLD_WIDTH;
    	new_food->pos_y = rand()%WORLD_HEIGHT;
    	new_food->next = food;
    	food = new_food;
    }

    meat = NULL;
}

void world_update()
{
    world_timestamp++;
    struct species_list *specimen = species;
    struct species_list *next;
    while (specimen != NULL)
    {
        next = specimen->next;
        update_subject_1(specimen->subject);
        specimen = next;
    }
    world_species_destroy();
}

struct species_list** world_species_return()
{
    return &species;
}

struct food_list* world_food_return()
{
    return food;
}

struct meat_list* world_meat_return()
{
    return meat;
}

uint32_t world_timestamp_return()
{
    return world_timestamp;
}


void world_display(bool console_display)
{
    if (world_timestamp%100 == 0)
        printf("time : %d\n", world_timestamp/100);

    struct species_list *specimen = species;
    uint8_t i = 0;
    while (specimen != NULL)
    {
        i++;
        if (console_display)
            printf("individu %d : ", i);
        display_subject_1(specimen->subject, console_display*2);
        specimen = specimen->next;
    }

    i = 0;
    struct food_list *cur_food = food;
    while (cur_food != NULL)
    {
        i++;
        if (cur_food->timer == 0)
            graphic_draw_circle_colored(cur_food->pos_x, cur_food->pos_y, BASE_SIZE, 0, 255, 0);
        else
            cur_food->timer--;
        cur_food = cur_food->next;
    }

    i = 0;
    struct meat_list *cur_meat = meat;
    struct meat_list *next_meat = meat;
    while (cur_meat != NULL)
    {
        next_meat = cur_meat->next;
        i++;
        if (cur_meat->timer != 0) {
            graphic_draw_circle_colored(cur_meat->pos_x, cur_meat->pos_y, BASE_SIZE, 255, 0, 0);
            cur_meat->timer--;
        } else {
            free_meat(cur_meat);
        }
        cur_meat = next_meat;
    }
}

void world_destroy()
{
    struct species_list *specimen = species;
    struct species_list *next_specimen;
    while (specimen != NULL)
    {
        next_specimen = specimen->next;
        release_species(specimen->subject);
        free(specimen);
        specimen = next_specimen;
    }

    struct food_list *cur_food = food;
    struct food_list *next_food;
    while (cur_food != NULL)
    {
        next_food = cur_food->next;
        free(cur_food);
        cur_food = next_food;
    }

}

void world_become_meat(struct species *subject)
{
    if (becoming_meat == NULL) {
        becoming_meat = (struct species_list*) malloc(sizeof(struct species_list));
        becoming_meat->subject = subject;
        becoming_meat->next = NULL;
    }
    else
    {
        struct species_list *first_meat = (struct species_list*) malloc(sizeof(struct species_list));
        first_meat->subject = subject;
        first_meat->next = becoming_meat;
        becoming_meat = first_meat;
    }
}

void world_species_destroy()
{
    while (becoming_meat != NULL)
    {
        struct species *subject = becoming_meat->subject;
        struct meat_list *new_meat = (struct meat_list*) malloc(sizeof(struct meat_list));
    	new_meat->pos_x = subject->pos_x;
    	new_meat->pos_y = subject->pos_y;
    	new_meat->timer = 64;
    	new_meat->next = meat;
    	meat = new_meat;

        struct species_list *cur_subject = species;
        struct species_list *last_subject = NULL;
        while (cur_subject != NULL && cur_subject->subject != subject)
        {
            last_subject = cur_subject;
            cur_subject = cur_subject->next;
        }
        if (cur_subject != NULL) {
            if (last_subject == NULL) {
                species = cur_subject->next;
            } else {
                last_subject->next = cur_subject->next;
            }
            release_species(cur_subject->subject);
            free(cur_subject);
        }
    struct species_list *temp = becoming_meat;
    becoming_meat = becoming_meat->next;
    free(temp);
    }
}

void free_meat(struct meat_list *meat_to_free)
{
    struct meat_list *cur_meat = meat;
    struct meat_list *last_meat = NULL;
    while (cur_meat != NULL && cur_meat != meat_to_free)
    {
        last_meat = cur_meat;
        cur_meat = cur_meat->next;
    }
    if (cur_meat != NULL) {
        if (last_meat == NULL) {
            meat = cur_meat->next;
        } else {
            last_meat->next = cur_meat->next;
        }
        free(cur_meat);
    }
}
