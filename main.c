#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 450

#define FPS 60
#define PARTICLES_AMOUNT 3000

typedef struct
{
    Vector2 p;  // position
    float r;    // radius
    float vx;   // velocity x
    float vy;   // velocity y
} Particle;

Color colors[] = { RED, GREEN, BLUE, ORANGE, PURPLE, GOLD };

Particle particles[PARTICLES_AMOUNT];

void DrawParticle(Particle *particle)
{
    DrawCircle(particle->p.x, particle->p.y, particle->r, WHITE);
}

void UpdateParticle(Particle *particle)
{
    particle->p.x += particle->vx;
    particle->p.y += particle->vy;

    float x = particle->p.x;
    float y = particle->p.y;
    float r = particle->r;
    
    // Handle boundary collisions
    if (x < 0 + r)
    {
        particle->p.x = r;
        particle->vx = -particle->vx;
    }
    if (x > WIDTH - r)
    {
        particle->p.x = WIDTH - r;
        particle->vx = -particle->vx;
    }
    if (y < 0 + r)
    {
        particle->p.y = r;
        particle->vy = -particle->vy;
    }
    if (y > HEIGHT - r)
    {
        particle->p.y = HEIGHT - r;
        particle->vy = -particle->vy;
    }
}

void DrawParticles()
{
    for (int i = 0; i < PARTICLES_AMOUNT; i++)
        DrawParticle(particles+i);
}

void UpdateParticles()
{
    for (int i = 0; i < PARTICLES_AMOUNT; i++)
        UpdateParticle(particles+i);
}

void HandleCollision(int index_a, int index_b)
{
    Particle* p1 = &particles[index_a];
    Particle* p2 = &particles[index_b];

    Vector2 vector_difference = { (p1->p.x - p2->p.x), (p1->p.y - p2->p.y) };
    float dx = vector_difference.x;
    float dy = vector_difference.y;

    float current_distance = sqrt(pow(dx, 2) + pow(dy, 2));
    if (current_distance == 0.0f) return;

    float r1 = particles[index_a].r;
    float r2 = particles[index_b].r;

    float total_radius = r1 + r2;

    if (current_distance < total_radius)
    {
        float overlap_amount = total_radius - current_distance;

        float nx = dx / current_distance;
        float ny = dy / current_distance;

        particles[index_a].p.x += nx * (overlap_amount / 2.0f);
        particles[index_a].p.y += ny * (overlap_amount / 2.0f);

        particles[index_b].p.x -= nx * (overlap_amount / 2.0f);
        particles[index_b].p.y -= ny * (overlap_amount / 2.0f);

        dx = particles[index_a].p.x - particles[index_b].p.x;
        dy = particles[index_a].p.y - particles[index_b].p.y;

        current_distance = total_radius;
    }

    Vector2 normal_vector = { (dx / current_distance), (dy / current_distance) };
    Vector2 tangent_vector = { -normal_vector.y, normal_vector.x };
        
    float v1_normal = (p1->vx * normal_vector.x) + (p1->vy * normal_vector.y);
    float v1_tangent = (p1->vx * tangent_vector.x) + (p1->vy * tangent_vector.y);

    float v2_normal = (p2->vx * normal_vector.x) + (p2->vy * normal_vector.y);
    float v2_tangent = (p2->vx * tangent_vector.x) + (p2->vy * tangent_vector.y);

    float v1_normal_prime = v2_normal;
    float v2_normal_prime = v1_normal;

    p1->vx = (v1_normal_prime * normal_vector.x) + (v1_tangent * tangent_vector.x);
    p1->vy = (v1_normal_prime * normal_vector.y) + (v1_tangent * tangent_vector.y);

    p2->vx = (v2_normal_prime * normal_vector.x) + (v2_tangent * tangent_vector.x);
    p2->vy = (v2_normal_prime * normal_vector.y) + (v2_tangent * tangent_vector.y);
}

void CheckAllCollisions()
{
    for (int i = 0; i < PARTICLES_AMOUNT - 1; i++)
    {
        for (int j = i + 1; j < PARTICLES_AMOUNT; j++)
        {
            if (CheckCollisionCircles(particles[i].p, particles[i].r, particles[j].p, particles[j].r))
            {
                HandleCollision(i, j);
            }
        }
    }
}

void InitParticles()
{
    for (int i = 0; i < PARTICLES_AMOUNT; i++)
    {
        int radius = 10;
        
        particles[i].r = radius;
        particles[i].p = (Vector2){ (float)GetRandomValue(radius, WIDTH-radius),
                                    (float)GetRandomValue(radius, HEIGHT-radius)};
        particles[i].vx = GetRandomValue(-5,5);
        particles[i].vy = GetRandomValue(-5,5);
    }
}

int main(void)
{
    const int current_fps_x = 0;
    const int current_fps_y = 0;

    InitWindow(WIDTH, HEIGHT, "simulation");
    InitParticles();

    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(BLACK);
            UpdateParticles();
            CheckAllCollisions();
            DrawParticles();
            DrawFPS(current_fps_x, current_fps_y);
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}

