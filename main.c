#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>

#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define SUB_STEPS 4

#define FPS 60
#define PARTICLES_AMOUNT 800
#define GRAVITY 0.025f
#define COEFFICIENT_OF_RESTITUTION 0.75f
#define EPSILON_MARGIN 0.01f

#define RADIUS 10

typedef struct
{
    Vector2 p;  // position
    float r;    // radius
    float vx;   // velocity x
    float vy;   // velocity y
} Particle;

Color colors[] = { YELLOW, ORANGE, RED, GREEN, BLUE, PURPLE, WHITE };
int paletteSize = sizeof(colors) / sizeof(colors[0]);

Particle particles[PARTICLES_AMOUNT];

void DrawParticle(Particle *particle, int index)
{
    DrawCircle(particle->p.x, particle->p.y, particle->r, colors[index % paletteSize]);
}


void HandleCollision(int index_a, int index_b)
{
    if (index_a == index_b) return;

    Particle* p1 = &particles[index_a];
    Particle* p2 = &particles[index_b];

    Vector2 vector_difference = { (p1->p.x - p2->p.x), (p1->p.y - p2->p.y) };
    float dx = vector_difference.x;
    float dy = vector_difference.y;

    float current_distance = sqrt((dx * dx) + (dy * dy));
    
    float r1 = p1->r;
    float r2 = p2->r;

    float total_radius = r1 + r2;

    if (current_distance < EPSILON_MARGIN)
    {
        float nx = 0;
        float ny = 1;

        current_distance = EPSILON_MARGIN;

        float overlap_amount = total_radius - current_distance;
        
        p1->p.x += nx * (overlap_amount / 2.0f);
        p1->p.y += ny * (overlap_amount / 2.0f);
        
        p2->p.x -= nx * (overlap_amount / 2.0f);
        p2->p.y -= ny * (overlap_amount / 2.0f);
    
        return;
    }
    else if (current_distance < total_radius)
    {
        float nx = dx / current_distance;
        float ny = dy / current_distance;
        
        float overlap_amount = total_radius - current_distance;

        p1->p.x += nx * (overlap_amount / 2.0f);
        p1->p.y += ny * (overlap_amount / 2.0f);

        p2->p.x -= nx * (overlap_amount / 2.0f);
        p2->p.y -= ny * (overlap_amount / 2.0f);

        Vector2 normal_vector = { nx, ny };
        Vector2 tangent_vector = { -ny, nx };

        float v1_normal = (p1->vx * normal_vector.x) + (p1->vy * normal_vector.y);
        float v1_tangent = (p1->vx * tangent_vector.x) + (p1->vy * tangent_vector.y);

        float v2_normal = (p2->vx * normal_vector.x) + (p2->vy * normal_vector.y);
        float v2_tangent = (p2->vx * tangent_vector.x) + (p2->vy * tangent_vector.y);

        // Check if they are moving toward each other
        if ((v1_normal - v2_normal) < 0)
        {
            float v1_normal_prime = (((1 - COEFFICIENT_OF_RESTITUTION) / 2) * v1_normal) + (((1 + COEFFICIENT_OF_RESTITUTION) / 2) * v2_normal);
            float v2_normal_prime = (((1 + COEFFICIENT_OF_RESTITUTION) / 2) * v1_normal) + (((1 - COEFFICIENT_OF_RESTITUTION) / 2) * v2_normal);
        
            p1->vx = (v1_normal_prime * normal_vector.x) + (v1_tangent * tangent_vector.x);
            p1->vy = (v1_normal_prime * normal_vector.y) + (v1_tangent * tangent_vector.y);

            p2->vx = (v2_normal_prime * normal_vector.x) + (v2_tangent * tangent_vector.x);
            p2->vy = (v2_normal_prime * normal_vector.y) + (v2_tangent * tangent_vector.y);

        }
    }
}

void UpdateParticle(Particle *particle)
{
    particle->vy += GRAVITY;

    particle->p.x += particle->vx;
    particle->p.y += particle->vy;

    float x = particle->p.x;
    float y = particle->p.y;
    float r = particle->r;

    // Handle boundary collisions
    if (x < 0 + r)
    {
        particle->p.x = r;
        //particle->vx = -particle->vx;
        
        particle->vx = -COEFFICIENT_OF_RESTITUTION * particle->vx;

    }
    if (x > WIDTH - r)
    {
        particle->p.x = WIDTH - r;
        //particle->vx = -particle->vx;
    
        particle->vx = -COEFFICIENT_OF_RESTITUTION * particle->vx;
    }
    if (y < 0 + r)
    {
        particle->p.y = r;
        //particle->vy = -particle->vy;
    
        particle->vy = -COEFFICIENT_OF_RESTITUTION * particle->vy;
    }
    if (y > HEIGHT - r)
    {
        particle->p.y = HEIGHT - r;
        //particle->vy = -particle->vy;
        
        particle->vy = -COEFFICIENT_OF_RESTITUTION * particle->vy;
    }
}

void DrawParticles()
{
    for (int i = 0; i < PARTICLES_AMOUNT; i++)
        DrawParticle(particles+i, i);
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

void UpdateParticles()
{
    float dt = 1.0f / SUB_STEPS;

    for (int step = 0; step < SUB_STEPS; step++)
    {
        for (int i = 0; i < PARTICLES_AMOUNT; i++)
        {
            particles[i].vy += GRAVITY * dt;

            particles[i].p.x += particles[i].vx * dt;
            particles[i].p.y += particles[i].vy * dt;

            float r = particles[i].r;

            if (particles[i].p.x < r)
            {
                particles[i].p.x = r;
                particles[i].vx = -COEFFICIENT_OF_RESTITUTION * particles[i].vx;
            }
            if (particles[i].p.x > WIDTH - r)
            {
                particles[i].p.x = WIDTH - r;
                particles[i].vx = -COEFFICIENT_OF_RESTITUTION * particles[i].vx;
            }
            if (particles[i].p.y < r)
            {
                particles[i].p.y = r;
                particles[i].vy = -COEFFICIENT_OF_RESTITUTION * particles[i].vy;
            }
            if (particles[i].p.y > HEIGHT - r)
            {
                particles[i].p.y = HEIGHT - r;
                particles[i].vy = -COEFFICIENT_OF_RESTITUTION * particles[i].vy;
            }
        }

        CheckAllCollisions();
    }
}

void InitParticles()
{
    for (int i = 0; i < PARTICLES_AMOUNT; i++)
    {
        int radius = RADIUS;
        
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
            DrawParticles();
            DrawFPS(current_fps_x, current_fps_y);
        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}

