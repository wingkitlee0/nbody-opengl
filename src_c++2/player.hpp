#pragma once

#include "vector.hpp"

class Player 
{
public:
    Vector position;
    Vector velocity;
    Vector acceleration;

    float inclination;
    float azimuth;

    Player();
    ~Player();
    void rotate(float inclination, float azimuth);
    void move_camera();
    void rotate_camera();
    void unrotate_camera();
};