#pragma once
#include <vector>
#include "raylib.h"

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Color color;
    float size;
    float lifetime;   // total lifetime
    float age;         // current age
    float rotation;    // visual spin
    float rotSpeed;    // spin speed
};

class ParticleSystem {
public:
    ParticleSystem();

    // Emitters
    void emitPlacement(float x, float y, float z, Color baseColor, int count = 12);
    void emitWinCelebration(const std::vector<std::pair<float, float>>& positions);
    void emitLanding(float x, float z, Color baseColor);

    // Update all particles (call each frame)
    void update(float dt);

    // Draw all particles in 3D mode (call inside BeginMode3D/EndMode3D)
    void draw() const;

    // Remove all particles
    void clear();

    bool empty() const { return particles.empty(); }

private:
    std::vector<Particle> particles;

    // Helper: random float in range
    static float randRange(float lo, float hi);

    // Helper: color with random variation
    static Color varyColor(Color base, int range);
};
