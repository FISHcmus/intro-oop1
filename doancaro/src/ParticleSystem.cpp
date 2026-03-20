#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>

ParticleSystem::ParticleSystem() {}

float ParticleSystem::randRange(float lo, float hi) {
    float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return lo + t * (hi - lo);
}

Color ParticleSystem::varyColor(Color base, int range) {
    auto clamp = [](int v) -> unsigned char {
        if (v < 0) return 0;
        if (v > 255) return 255;
        return static_cast<unsigned char>(v);
    };
    return {
        clamp(base.r + (std::rand() % (2 * range + 1)) - range),
        clamp(base.g + (std::rand() % (2 * range + 1)) - range),
        clamp(base.b + (std::rand() % (2 * range + 1)) - range),
        base.a
    };
}

void ParticleSystem::emitPlacement(float x, float y, float z, Color baseColor, int count) {
    for (int i = 0; i < count; i++) {
        Particle p{};
        p.position = {x, y + 0.1f, z};
        // Burst outward in a ring
        float angle = randRange(0.0f, 2.0f * 3.14159f);
        float speed = randRange(1.5f, 4.0f);
        p.velocity = {
            std::cos(angle) * speed,
            randRange(1.0f, 3.5f),   // upward
            std::sin(angle) * speed
        };
        p.color = varyColor(baseColor, 30);
        p.size = randRange(0.04f, 0.10f);
        p.lifetime = randRange(0.4f, 0.8f);
        p.age = 0.0f;
        p.rotation = randRange(0.0f, 360.0f);
        p.rotSpeed = randRange(-200.0f, 200.0f);
        particles.push_back(p);
    }
}

void ParticleSystem::emitLanding(float x, float z, Color baseColor) {
    // Small dust puff at base
    for (int i = 0; i < 6; i++) {
        Particle p{};
        float angle = randRange(0.0f, 2.0f * 3.14159f);
        float speed = randRange(0.5f, 1.5f);
        p.position = {x, 0.05f, z};
        p.velocity = {
            std::cos(angle) * speed,
            randRange(0.2f, 0.8f),
            std::sin(angle) * speed
        };
        p.color = {200, 200, 200, 180};  // light grey dust
        p.size = randRange(0.03f, 0.07f);
        p.lifetime = randRange(0.3f, 0.5f);
        p.age = 0.0f;
        p.rotation = 0.0f;
        p.rotSpeed = 0.0f;
        particles.push_back(p);
    }
}

void ParticleSystem::emitWinCelebration(const std::vector<std::pair<float, float>>& positions) {
    // Big confetti explosion from each winning cell
    Color confettiColors[] = {
        {255, 215, 0, 255},    // gold
        {255, 69, 0, 255},     // orange-red
        {0, 191, 255, 255},    // deep sky blue
        {50, 205, 50, 255},    // lime green
        {255, 105, 180, 255},  // hot pink
        {148, 103, 255, 255},  // purple
        {255, 255, 100, 255},  // yellow
    };
    int numColors = sizeof(confettiColors) / sizeof(confettiColors[0]);

    for (const auto& pos : positions) {
        float x = pos.first;
        float z = pos.second;

        for (int i = 0; i < 20; i++) {
            Particle p{};
            p.position = {x, 0.3f, z};
            float angle = randRange(0.0f, 2.0f * 3.14159f);
            float speed = randRange(2.0f, 6.0f);
            p.velocity = {
                std::cos(angle) * speed,
                randRange(3.0f, 8.0f),   // high upward burst
                std::sin(angle) * speed
            };
            p.color = confettiColors[std::rand() % numColors];
            p.size = randRange(0.06f, 0.14f);
            p.lifetime = randRange(1.0f, 2.0f);
            p.age = 0.0f;
            p.rotation = randRange(0.0f, 360.0f);
            p.rotSpeed = randRange(-300.0f, 300.0f);
            particles.push_back(p);
        }
    }
}

void ParticleSystem::update(float dt) {
    for (auto& p : particles) {
        p.age += dt;
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.position.z += p.velocity.z * dt;

        // Gravity
        p.velocity.y -= 9.8f * dt;

        // Spin
        p.rotation += p.rotSpeed * dt;

        // Shrink over lifetime
        float lifeRatio = p.age / p.lifetime;
        if (lifeRatio > 0.7f) {
            p.size *= (1.0f - 2.0f * dt);  // fade shrink near end
        }
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                        [](const Particle& p) { return p.age >= p.lifetime; }),
        particles.end());
}

void ParticleSystem::draw() const {
    for (const auto& p : particles) {
        float lifeRatio = p.age / p.lifetime;
        // Fade alpha over lifetime
        unsigned char alpha = static_cast<unsigned char>(
            static_cast<float>(p.color.a) * (1.0f - lifeRatio));
        Color c = {p.color.r, p.color.g, p.color.b, alpha};

        DrawCube(p.position, p.size, p.size, p.size, c);
    }
}

void ParticleSystem::clear() {
    particles.clear();
}
