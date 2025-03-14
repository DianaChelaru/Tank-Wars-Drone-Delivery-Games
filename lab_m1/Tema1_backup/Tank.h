#pragma once
#include <glm/glm.hpp>

struct Tank {
    float x;
    float y;
    float angle;
    float radius = 20;
    float health;
    bool is_alive;
    glm::vec3 healthColor = glm::vec3(0, 1, 0);
};
