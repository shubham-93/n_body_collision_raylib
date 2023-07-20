/*
Simulation of n particles of random masses, positions and velocities colliding
elastically. Made with raylib.

Author: Shubham Maheshwari
email: shubham.93@gmail.com
GitHub: https://github.com/shubham-93
*/

#include "raylib.h"
#include "raymath.h"
#include "gsl/gsl_rng.h"

int main(void) {

    // Define a struct called particle that holds information about its position, velocity, radius and mass
    typedef struct {
        Vector2 position;
        Vector2 velocity;
        int radius;
        int mass;
    } particle;


    // Initialize particle radius
    const size_t particle_radius = 8;

    // Initialize particle mass
    // const size_t particle_mass = 10;

    // Array of particles of length = number_of_particles
    const size_t number_of_particles = 50;
    particle particle_array[number_of_particles];

    // Initialize Random Number Generator
    gsl_rng *r = gsl_rng_alloc(gsl_rng_taus);

    // Initialize screen width and height
    const int screenWidth = 1000;
    const int screenHeight = 600;

    bool pause = 0;
    int framesCounter = 0;

    SetTargetFPS(60);

    // Initialize window and OpenGL context
    InitWindow(screenWidth, screenHeight, "Colliding particles");

    // Initialize particle positions, velocities and radii
    for (int particle_label = 0; particle_label < number_of_particles; particle_label++) {
        
        // Initialize random particle positions
        particle_array[particle_label].position = (Vector2){gsl_rng_uniform_int(r, GetScreenWidth()),\
                                                            gsl_rng_uniform_int(r, GetScreenHeight())};

        // Initialize random particle velocities
        particle_array[particle_label].velocity = (Vector2){(10*(gsl_rng_uniform(r) + 0.1)) + (-10*(gsl_rng_uniform(r) + 0.1)),\
                                                            (10*(gsl_rng_uniform(r) + 0.1)) + (-10*(gsl_rng_uniform(r) + 0.1))};

        // Initialize random particle masses
        particle_array[particle_label].mass = 1 + gsl_rng_uniform_int(r, 9);
        // particle_array[particle_label].mass = particle_mass;

        // Initialize particle radius (for all particles)
        particle_array[particle_label].radius = particle_radius;
    }

    // Free RNG memory
    gsl_rng_free(r);


    // Main animation loop. One frame per loop
    // Detect window close button or ESC key
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) pause = !pause;

        if (!pause) {
            // Update particle positions. Each time step is equal to 1 in appropriate units.
            // Each frame represents a time step of 1 unit.
            // Since the simulation is at 60 FPS, this means that it is 60 times faster than 
            // what it would be in real life.
            for (int particle_label = 0; particle_label < number_of_particles; particle_label++) {
                
                particle_array[particle_label].position.x += particle_array[particle_label].velocity.x;
                particle_array[particle_label].position.y += particle_array[particle_label].velocity.y;
                // By this point, we have updated all particle positions
            }
        
            // We now check for wall collisions. We should put this loop here, not above, because after checking for wall collisions,
            // we will check for collisions between particles. And collisions between particles should be checked AFTER all particles have moved forward
            // in time.
            for (int particle_label = 0; particle_label < number_of_particles; particle_label++) {

                if (((particle_array[particle_label].position.x >= (GetScreenWidth() - particle_array[particle_label].radius)) && (particle_array[particle_label].velocity.x>0)) || ((particle_array[particle_label].position.x <= particle_array[particle_label].radius) && (particle_array[particle_label].velocity.x<0))) {
                    particle_array[particle_label].velocity.x *= -1.0f;
                }

                if (((particle_array[particle_label].position.y >= (GetScreenHeight() - particle_array[particle_label].radius)) && (particle_array[particle_label].velocity.y>0)) || ((particle_array[particle_label].position.y <= particle_array[particle_label].radius) && (particle_array[particle_label].velocity.y<0))) {
                    particle_array[particle_label].velocity.y *= -1.0f;
                }
                // By this point, we have checked for wall collisions and updated the particle velocities accordingly    
            }

            // Check collisions between every pair of particles
            for (int particle_one_label = 0; particle_one_label < number_of_particles; particle_one_label++) {
                for (int particle_two_label = particle_one_label+1; particle_two_label < number_of_particles; particle_two_label++) {
                    // [Bug][Fixed] Fixed segfault because of buffer overflow. In the 2nd loop, I accidentally used particle_one_label < number_of_particles
                    // instead of the correct particle_two_label < number_of_particles, which caused particle_array to overflow

                    // Define relative displacement and velocity vectors, e.g. r_12 = relative displacement vector of particle 1 with respect to 2
                    Vector2 r_12 = Vector2Subtract(particle_array[particle_one_label].position, particle_array[particle_two_label].position);
                    Vector2 v_12 = Vector2Subtract(particle_array[particle_one_label].velocity, particle_array[particle_two_label].velocity);
                    Vector2 r_21 = Vector2Negate(r_12);
                    Vector2 v_21 = Vector2Negate(v_12);

                    // Check if distance between 2 particles is smaller than the sum of their radii
                    // and also check if they are approaching each other
                    if ((Vector2Length(r_12) < 2.0f*(float)particle_radius) && (Vector2DotProduct(r_12, v_12) < 0.0f)) {
                        // [Bug][Fixed] I'm getting a floating point exception: 8 in the below 2 lines. Reason and fix: Mass of some particles were zero because I forgot to notice that RNG can output zero
                        //[Bug] [Fixed] The directions of particles after colliding are fine, but somehow they gain speed in every collision. Fix: typo in the velocity update formula
                        particle_array[particle_one_label].velocity = Vector2Subtract(particle_array[particle_one_label].velocity , Vector2Scale(r_12, (float)(2*particle_array[particle_two_label].mass/(particle_array[particle_one_label].mass + particle_array[particle_two_label].mass)) * (Vector2DotProduct(v_12, r_12)/Vector2LengthSqr(r_12))));
                        particle_array[particle_two_label].velocity = Vector2Subtract(particle_array[particle_two_label].velocity , Vector2Scale(r_21, (float)(2*particle_array[particle_one_label].mass/(particle_array[particle_one_label].mass + particle_array[particle_two_label].mass)) * (Vector2DotProduct(v_21, r_21)/Vector2LengthSqr(r_21))));
                        
                    }
                
                }
            
            }

        }
        else framesCounter++;


        // We now draw all particles based on positions calculated above
        BeginDrawing();

        ClearBackground(RAYWHITE);
        // Draw particles
        for (int particle_label = 0; particle_label < number_of_particles; particle_label++) {
            DrawCircleV(particle_array[particle_label].position, (float)particle_array[particle_label].radius, MAROON);
        }

        // Draw text to show how to quit or pause
        DrawText("Quit: Esc | Pause: Space", 10, GetScreenHeight()-25, 20, LIGHTGRAY);

        // On pause, we draw a blinking message
        if (pause && ((framesCounter/30)%2)) DrawText("Paused", GetScreenWidth()/2, GetScreenHeight()/2, 30, GRAY);

        // Show FPS
        DrawFPS(10, 10);
    
        EndDrawing();
        // End of animation loop
        // Go back to beginning of loop, and repeat for next frame
    }
    // Close window and unload OpenGL context
    CloseWindow();

    return EXIT_SUCCESS;
}