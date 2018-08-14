#include <stdlib.h>
#include <math.h>
#include "star.hpp"

float randomFloat() {
    return ((float) rand()) / ((float) RAND_MAX);
}


Vector randomVector() {
    return Vector_init(
        (2.0f * randomFloat()) - 1.0f,
        (2.0f * randomFloat()) - 1.0f,
        (2.0f * randomFloat()) - 1.0f
    );
}

Star Star_create_random()
{
    /* create a star with random x, y, z, vx, vy, vz, m */
    Star result;
    result.mass = randomFloat();
    result.position = randomVector();
    result.velocity = Vector_scale(randomVector(), 0.2f);
    return result;
}

Star Star_create_random_flat() 
{    
    /* create a star with random x, y, vx, vy, m, but z=vz=0 */
    Star result;
    result.mass = randomFloat();
    result.position = randomVector();
    result.velocity = Vector_scale(randomVector(), 0.2f);
    result.position.z = 0.0f;
    result.velocity.z = 0.0f;
    return result;
}

Star Star_create_random_ring() 
{    
    /* create a ring of stars */
    Star result;
    result.mass = randomFloat();
    float theta = randomFloat() * 2.0f * M_PI;
    float rad = randomFloat() * 0.01f + 1.0f;
    result.position.x = rad * cos(theta);
    result.position.y = rad * sin(theta);
    result.position.z = 0.0f;

    result.velocity.x = -sqrt(0.1f) * result.position.y;
    result.velocity.y = sqrt(0.1f) * result.position.x;
    result.velocity.z = 0.0f;
    return result;
}

float* load_star_texture() {
    //float* texture = calloc(STAR_TEXTURE_SIZE * STAR_TEXTURE_SIZE, sizeof(float));
    float* texture = new float[STAR_TEXTURE_SIZE * STAR_TEXTURE_SIZE];
    float radius = ((float) STAR_TEXTURE_SIZE) / 2.0f;
    float sigma_2 = 8.0f;

    for (size_t i = 0; i < STAR_TEXTURE_SIZE; ++i) {
        for (size_t j = 0; j < STAR_TEXTURE_SIZE; ++j) {
            float x = ((float) i) - radius;
            float y = ((float) j) - radius;

            float exponent = ((x * x) / (2.0f * sigma_2)) + ((y * y) / (2.0f * sigma_2));
            float alpha = expf(-exponent);

            size_t index = i + (STAR_TEXTURE_SIZE * j);
            texture[index] = alpha;
        }
    }
    return texture;
}
