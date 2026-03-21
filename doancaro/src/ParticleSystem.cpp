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
    // Bright sparkles that float upward and twinkle
    for (int i = 0; i < count; i++) {
        Particle p{};
        float angle = randRange(0.0f, 2.0f * PI);
        float spread = randRange(0.3f, 1.8f);
        p.position = {x, y + 0.1f, z};
        p.velocity = {
            std::cos(angle) * spread,
            randRange(1.5f, 4.0f),
            std::sin(angle) * spread
        };
        // Brighten base color toward white for sparkle effect
        Color bright = {
            static_cast<unsigned char>(std::min(255, baseColor.r + 80)),
            static_cast<unsigned char>(std::min(255, baseColor.g + 80)),
            static_cast<unsigned char>(std::min(255, baseColor.b + 80)),
            255
        };
        p.color = varyColor(bright, 20);
        p.size = randRange(0.03f, 0.08f);
        p.lifetime = randRange(0.5f, 1.0f);
        p.age = 0.0f;
        p.rotation = randRange(0.0f, 360.0f);
        p.rotSpeed = randRange(-300.0f, 300.0f);
        p.shape = ParticleShape::Spark;
        p.shimmerPhase = randRange(0.0f, 2.0f * PI);
        particles.push_back(p);
    }

    // A few larger glowing orbs that rise slowly
    for (int i = 0; i < 4; i++) {
        Particle p{};
        float angle = randRange(0.0f, 2.0f * PI);
        p.position = {x, y + 0.05f, z};
        p.velocity = {
            std::cos(angle) * randRange(0.2f, 0.6f),
            randRange(0.8f, 1.5f),
            std::sin(angle) * randRange(0.2f, 0.6f)
        };
        p.color = {255, 255, 240, 200};  // warm white glow
        p.size = randRange(0.06f, 0.12f);
        p.lifetime = randRange(0.6f, 1.2f);
        p.age = 0.0f;
        p.rotation = 0.0f;
        p.rotSpeed = 0.0f;
        p.shape = ParticleShape::Cube;
        p.shimmerPhase = 0.0f;
        particles.push_back(p);
    }
}

void ParticleSystem::emitLanding(float x, float z, Color baseColor) {
    // Ring of dust puffs at base
    for (int i = 0; i < 8; i++) {
        Particle p{};
        float angle = randRange(0.0f, 2.0f * PI);
        float speed = randRange(0.8f, 2.0f);
        p.position = {x, 0.05f, z};
        p.velocity = {
            std::cos(angle) * speed,
            randRange(0.1f, 0.5f),
            std::sin(angle) * speed
        };
        p.color = varyColor({180, 170, 150, 160}, 15);  // warm dust
        p.size = randRange(0.04f, 0.09f);
        p.lifetime = randRange(0.25f, 0.45f);
        p.age = 0.0f;
        p.rotation = 0.0f;
        p.rotSpeed = 0.0f;
        p.shape = ParticleShape::Cube;
        p.shimmerPhase = 0.0f;
        particles.push_back(p);
    }
}

void ParticleSystem::emitWinCelebration(const std::vector<std::pair<float, float>>& positions) {
    static const Color confettiColors[] = {
        {255, 215, 0, 255},    // gold
        {255, 69, 0, 255},     // orange-red
        {0, 191, 255, 255},    // deep sky blue
        {50, 205, 50, 255},    // lime green
        {255, 105, 180, 255},  // hot pink
        {148, 103, 255, 255},  // purple
        {255, 255, 100, 255},  // yellow
        {255, 255, 255, 255},  // white
    };
    static const int numColors = sizeof(confettiColors) / sizeof(confettiColors[0]);

    // Confetti burst from each winning cell
    for (const auto& pos : positions) {
        float x = pos.first;
        float z = pos.second;

        // Tumbling flat rectangles (confetti)
        for (int i = 0; i < 25; i++) {
            Particle p{};
            p.position = {x, 0.3f, z};
            float angle = randRange(0.0f, 2.0f * PI);
            float speed = randRange(2.5f, 7.0f);
            p.velocity = {
                std::cos(angle) * speed,
                randRange(4.0f, 10.0f),
                std::sin(angle) * speed
            };
            p.color = confettiColors[std::rand() % numColors];
            p.size = randRange(0.06f, 0.14f);
            p.lifetime = randRange(1.5f, 3.0f);
            p.age = 0.0f;
            p.rotation = randRange(0.0f, 360.0f);
            p.rotSpeed = randRange(-400.0f, 400.0f);
            p.shape = ParticleShape::FlatRect;
            p.shimmerPhase = 0.0f;
            particles.push_back(p);
        }

        // Sparkle burst (smaller, twinkling)
        for (int i = 0; i < 10; i++) {
            Particle p{};
            p.position = {x, 0.4f, z};
            float angle = randRange(0.0f, 2.0f * PI);
            float speed = randRange(1.0f, 3.5f);
            p.velocity = {
                std::cos(angle) * speed,
                randRange(2.0f, 6.0f),
                std::sin(angle) * speed
            };
            p.color = {255, 255, 220, 255};  // warm white sparkle
            p.size = randRange(0.03f, 0.07f);
            p.lifetime = randRange(0.8f, 1.8f);
            p.age = 0.0f;
            p.rotation = randRange(0.0f, 360.0f);
            p.rotSpeed = randRange(-200.0f, 200.0f);
            p.shape = ParticleShape::Spark;
            p.shimmerPhase = randRange(0.0f, 2.0f * PI);
            particles.push_back(p);
        }
    }

    // Extra screen-wide confetti rain (falls from above the board center)
    float cx = 0.0f, cz = 0.0f;
    for (const auto& pos : positions) {
        cx += pos.first;
        cz += pos.second;
    }
    if (!positions.empty()) {
        cx /= static_cast<float>(positions.size());
        cz /= static_cast<float>(positions.size());
    }
    for (int i = 0; i < 40; i++) {
        Particle p{};
        p.position = {
            cx + randRange(-4.0f, 4.0f),
            randRange(5.0f, 9.0f),
            cz + randRange(-4.0f, 4.0f)
        };
        p.velocity = {
            randRange(-1.0f, 1.0f),
            randRange(-1.0f, 1.0f),
            randRange(-1.0f, 1.0f)
        };
        p.color = confettiColors[std::rand() % numColors];
        p.size = randRange(0.05f, 0.12f);
        p.lifetime = randRange(2.0f, 4.0f);
        p.age = 0.0f;
        p.rotation = randRange(0.0f, 360.0f);
        p.rotSpeed = randRange(-250.0f, 250.0f);
        p.shape = ParticleShape::FlatRect;
        p.shimmerPhase = 0.0f;
        particles.push_back(p);
    }
}

void ParticleSystem::update(float dt) {
    float time = static_cast<float>(GetTime());

    for (auto& p : particles) {
        p.age += dt;
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.position.z += p.velocity.z * dt;

        // Gravity — lighter for confetti (air resistance)
        float gravity = (p.shape == ParticleShape::FlatRect) ? 4.0f : 9.8f;
        p.velocity.y -= gravity * dt;

        // Air drag for confetti — makes them flutter
        if (p.shape == ParticleShape::FlatRect) {
            float drag = 0.97f;
            p.velocity.x *= drag;
            p.velocity.z *= drag;
            // Gentle side drift
            p.velocity.x += std::sin(time * 3.0f + p.shimmerPhase) * 0.5f * dt;
            p.velocity.z += std::cos(time * 2.5f + p.shimmerPhase) * 0.5f * dt;
        }

        // Sparks: gentle upward drift to counteract gravity a bit
        if (p.shape == ParticleShape::Spark) {
            p.velocity.y += 2.0f * dt;
        }

        // Spin
        p.rotation += p.rotSpeed * dt;

        // Shrink near end of life
        float lifeRatio = p.age / p.lifetime;
        if (lifeRatio > 0.7f) {
            p.size *= (1.0f - 2.0f * dt);
        }

        // Floor bounce for confetti
        if (p.shape == ParticleShape::FlatRect && p.position.y < 0.02f && p.velocity.y < 0.0f) {
            p.position.y = 0.02f;
            p.velocity.y *= -0.3f;  // lose energy
            p.rotSpeed *= 0.5f;
        }
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                        [](const Particle& p) { return p.age >= p.lifetime; }),
        particles.end());
}

void ParticleSystem::draw() const {
    float time = static_cast<float>(GetTime());

    for (const auto& p : particles) {
        float lifeRatio = p.age / p.lifetime;
        float fadeAlpha = static_cast<float>(p.color.a) * (1.0f - lifeRatio);

        switch (p.shape) {
            case ParticleShape::Spark: {
                // Twinkle effect: modulate alpha with sin wave
                float shimmer = (std::sin(time * 12.0f + p.shimmerPhase) + 1.0f) * 0.5f;
                float sparkAlpha = fadeAlpha * (0.4f + 0.6f * shimmer);
                auto a = static_cast<unsigned char>(sparkAlpha);
                Color c = {p.color.r, p.color.g, p.color.b, a};
                // Draw as small bright cube
                DrawCube(p.position, p.size, p.size, p.size, c);
                // Additive glow: slightly larger, dimmer
                auto ga = static_cast<unsigned char>(sparkAlpha * 0.3f);
                Color gc = {p.color.r, p.color.g, p.color.b, ga};
                DrawCube(p.position, p.size * 2.5f, p.size * 2.5f, p.size * 2.5f, gc);
                break;
            }
            case ParticleShape::FlatRect: {
                auto a = static_cast<unsigned char>(fadeAlpha);
                Color c = {p.color.r, p.color.g, p.color.b, a};
                // Flat tumbling rectangle
                float w = p.size * 1.5f;
                float h = p.size * 0.15f;
                float d = p.size * 0.8f;
                DrawCube(p.position, w, h, d, c);
                break;
            }
            case ParticleShape::Cube:
            default: {
                auto a = static_cast<unsigned char>(fadeAlpha);
                Color c = {p.color.r, p.color.g, p.color.b, a};
                DrawCube(p.position, p.size, p.size, p.size, c);
                break;
            }
        }
    }
}

void ParticleSystem::clear() {
    particles.clear();
}
