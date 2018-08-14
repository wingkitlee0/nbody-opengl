#include <stdio.h>
#include <SDL2/SDL.h>

#include "log.h"
#include "nbody.hpp"

NBody::NBody()
{
    window = new Window();
    if (window == NULL) {
        printf("error...");
    }

    player = new Player();
    player->position = Vector_init(0.0f, 0.0f, 5.0f);

    // Turn on alpha blending for transparency
    glEnable(GL_BLEND);  // Turn Blending On
    glEnable(GL_TEXTURE_2D);  // Turn on textures
    glDisable(GL_DEPTH_TEST);  // Turn Depth Testing Off
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Load up star texture
    glGenTextures(1, &this->star_texture);
    glBindTexture(GL_TEXTURE_2D, this->star_texture);

    float* pixels = load_star_texture();
    glTexImage2D(
        GL_TEXTURE_2D,      // target
        0,                  // level
        GL_ALPHA,           // internalFormat
        STAR_TEXTURE_SIZE,  // width
        STAR_TEXTURE_SIZE,  // height
        0,                  // border
        GL_ALPHA,           // format
        GL_FLOAT,           // type
        pixels              // data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(pixels);
    
    this->stars.resize(NUM_STARS);
    for (size_t i = 0; i < NUM_STARS; ++i) {
        this->stars[i] = Star_create_random();
    }
}

NBody::~NBody() 
{
    if (this->star_texture != 0) {
        glDeleteTextures(1, &this->star_texture);
        this->star_texture = 0;
    }

    if (player != NULL) delete player;
    if (window != NULL) delete window;

/*
    if (this->player != NULL) {
        Player_destroy(this->player);
        this->player = NULL;
    }

    if (this->window != NULL) {
        Window_destroy(this->window);
        this->window = NULL;
    }
*/
}

void NBody::NBody_run() {
    log_info("Running nbody...");

    this->running = true;
    uint32_t lastTicks = SDL_GetTicks();

    while (this->running) {
        uint32_t currentTicks = SDL_GetTicks();
        NBody_step(currentTicks - lastTicks);

        lastTicks = currentTicks;
        SDL_Delay(16);
    }

    log_info("Shutting down.");
}


void NBody::NBody_handle_event(SDL_Event event) {
    if(event.type == SDL_QUIT) {
        this->running = false;
        return;
    }

    if (event.type == SDL_KEYDOWN) {
        switch( event.key.keysym.sym ) {
            case SDLK_ESCAPE: this->running = false;
            default: break;
        }
        return;
    }
}


void NBody::NBody_handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        NBody_handle_event(event);
    }

    const uint8_t *keystates = SDL_GetKeyboardState(NULL);

    if (keystates[SDL_SCANCODE_UP]) {
        player->rotate(0.0f, 1.0f);
    }

    if (keystates[SDL_SCANCODE_DOWN]) {
        player->rotate(0.0f, -1.0f);
        //Player_rotate(this->player, 0.0f, -1.0f);
    }

    if (keystates[SDL_SCANCODE_RIGHT]) {
        player->rotate(1.0f, 0.0f);
        //Player_rotate(this->player, 1.0f, 0.0f);
    }

    if (keystates[SDL_SCANCODE_LEFT]) {
        //Player_rotate(this->player, -1.0f, 0.0f);
        player->rotate(-1.0f, 0.0f);
    }
}


void NBody::NBody_tick(uint32_t dt) {
    //printf("calling NBody_tick()");
    player->acceleration = Vector_init(0.0f, 0.0f, 0.0f);
    Vector accelerations[NUM_STARS] = {{0.0f, 0.0f, 0.0f}};

    // Update player
    for (size_t i = 0; i < NUM_STARS; ++i) {
        Star* star = &this->stars[i];
        /*
        if (i==0 && dt == 0) {
            star->mass = 10.0;
            star->position.x = 0.0;
            star->position.y = 0.0;
            star->position.z = 0.0;
        }*/

        Vector delta = Vector_subtract(star->position, this->player->position);
        float distance = Vector_mag(delta) + 0.00001f;  // Add small amount to avoid collision

        Vector force = Vector_scale(delta, 0.001f / (distance * distance * distance));
        this->player->acceleration = Vector_add(this->player->acceleration, Vector_scale(force, star->mass));
    }

    this->player->velocity = Vector_add(this->player->velocity, Vector_scale(this->player->acceleration, 0.001f * dt));
    this->player->position = Vector_add(this->player->position, Vector_scale(this->player->velocity, 0.001f * dt));

    // Update stars
    for (size_t i = 0; i < NUM_STARS; ++i) {
        Star* star_i = &this->stars[i];

        for (size_t j = i + 1; j < NUM_STARS; ++j) {
            Star* star_j = &this->stars[j];

            Vector delta = Vector_subtract(star_j->position, star_i->position);
            float distance = Vector_mag(delta) + 0.00001f;  // Add small amount to avoid collision

            Vector force = Vector_scale(delta, 0.001f / (distance * distance * distance));
            accelerations[i] = Vector_add(accelerations[i], Vector_scale(force, star_j->mass));
            accelerations[j] = Vector_add(accelerations[j], Vector_scale(force, -star_i->mass));
        }

        star_i->velocity = Vector_add(star_i->velocity, Vector_scale(accelerations[i], 0.001f * dt));
        star_i->position = Vector_add(star_i->position, Vector_scale(star_i->velocity, 0.001f * dt));
    }
}


void NBody::NBody_draw_star(Star star) {
    // Set stars color
    if (star.mass < 0.5f) {
        float lum = star.mass + 0.5f;
        float lum_2 = lum * lum;
        glColor4f(lum, lum_2, lum_2, 1.0f);
    } else {
        float lum = (1.0f - star.mass) + 0.5f;
        float lum_2 = lum * lum;
        glColor4f(lum_2, lum_2, lum, 1.0f);
    }

    glPushMatrix();
        // Translate to star's center
        glTranslatef(star.position.x, star.position.y, star.position.z);

        // Face stars towards camera
        //Player_unrotate_camera(this->player);
        player->unrotate_camera();

        // Scale star by mass
        float scale = 0.08f * sqrtf(star.mass);
        glScalef(scale, scale, scale);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, -1.0f, 1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f, 1.0f);
        glEnd();
    glPopMatrix();
}


void NBody::NBody_draw_stars() {
    // Select Our Texture
    glBindTexture(GL_TEXTURE_2D, this->star_texture);

    for (size_t i = 0; i < NUM_STARS; ++i) {
        NBody_draw_star(this->stars[i]);
    }
}


void loadPerspective(float fovyInDegrees, float znear, float zfar) {
    float ymax = znear * tanf(fovyInDegrees * M_PI / 360.0f);
    float xmax = ymax * (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT;
    glFrustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}


void NBody::NBody_draw() {
    // Clear The Screen And The Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    loadPerspective(60.0f, 0.0001f, 100.0f);

    // Reset model view projection
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move to camera to player
    //Player_move_camera(this->player);
    this->player->move_camera();

    NBody_draw_stars();
    //Window_update(this->window);
    this->window->Window_update();
}


void NBody::NBody_step(uint32_t dt) {
    NBody_handle_events();
    NBody_tick(dt);
    NBody_draw();
}


