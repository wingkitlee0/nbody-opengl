#pragma once

#include "vector.hpp"

#define STAR_TEXTURE_SIZE 16

class Star 
{
    /* a class for single star */
public:
    float mass;
    Vector position;
    Vector velocity;
};

Star Star_create_random(); // create a star with random x, y, z, vx, vy, vz, m
Star Star_create_random_flat(); // create a star with random x, y, vx, vy, m, but z=vz=0
Star Star_create_random_ring(); // create a ring of stars
float* load_star_texture(void);
