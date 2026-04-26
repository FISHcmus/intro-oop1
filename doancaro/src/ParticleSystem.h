#pragma once
#include <vector>
#include <deque>
#include "raylib.h"

enum class ParticleShape { Cube, FlatRect, Spark };

struct Particle {
    Vector3 position;
    Vector3 velocity;
    Color color;
    float size;
    float lifetime;
    float age;
    float rotation;
    float rotSpeed;
    ParticleShape shape;
    float shimmerPhase;  // for sparkle twinkle offset
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
    enum class BurstKind { Confetti, Sparkle };

    struct PendingBurst {
        float spawnAt;
        float x, z;
        BurstKind kind;
    };

    std::vector<Particle> particles;
    std::deque<PendingBurst> pendingBursts;
    float pendingClock = 0.0f;

    void spawnBurst(float x, float z, BurstKind kind);

    static float randRange(float lo, float hi);
    static Color varyColor(Color base, int range);
};
