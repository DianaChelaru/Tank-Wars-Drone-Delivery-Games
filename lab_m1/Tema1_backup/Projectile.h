#pragma once
#include <glm/glm.hpp>

struct Projectile {
    glm::vec2 pos;
    bool is_active;
    glm::vec2 velocity;
    float radius = 4;
    float damage = 10;
};
