#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"
#include "lab_m1/Tema1/Tema1.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    float y = 0, x = 0;
    tank1.x = 285;
    tank1.y = 0;
    tank1.angle = 0;
    tank1.health = 100;
    tank1.is_alive = true;
    cannon1.angle = 0;
    cannon1.pos = glm::vec2(tankCircle_radius * cos(cannon1.angle), tankCircle_radius * sin(cannon1.angle));
    projectile1.is_active = false;

    tank2.x = 1000;
    tank2.y = 0;
    tank2.angle = 0;
    tank2.health = 100;
    tank2.is_alive = true;
    cannon2.angle = 0;
    cannon2.pos = glm::vec2(tankCircle_radius * cos(cannon2.angle), tankCircle_radius * sin(cannon2.angle));
    projectile2.is_active = false;


    for (float i = 0; i < resolution.x; i += resolution.x / nrPoints) {
        y = 200 + 100 * sin(0.01 * i) + 150 * sin(0.0005 * i) + 50 * sin(0.025 * i);
        heightMap.push_back(glm::vec2(x, y));
        x++;
    }

    for (int i = 0; i < heightMap.size() - 1; ++i) {
        glm::vec2& A = heightMap[i];
        Mesh* square = (object2D::CreateSquare("square_" + std::to_string(i),glm::vec3(0,0,0), 1, glm::vec3(0.35, 0.64, 0.43), true));
        meshes["square_" + std::to_string(i)] = square;
    }

    Mesh* baseTrapezoid1 = (object2D::CreateTrapezoid("baseTrapezoid1", glm::vec3(0, 0, 0), base_baseTrapezoid, glm::vec3(0.35, 0.23, 0.008), true));
    meshes["baseTrapezoid1"] = baseTrapezoid1;
    Mesh* topTrapezoid1 = (object2D::CreateTrapezoid("topTrapezoid1", glm::vec3(0, 23, 0), -top_baseTrapezoid, glm::vec3(0.65, 0.55, 0.35), true));
    meshes["topTrapezoid1"] = topTrapezoid1;
    Mesh* circle1 = (object2D::CreateCircle("circle1", glm::vec3(0, 23, 0), tankCircle_radius, 70, glm::vec3(0.65, 0.55, 0.35), true));
    meshes["circle1"] = circle1;
    Mesh* t1 = (object2D::CreateRectangle("cannon1", glm::vec3(0, 30, 0), cannonWidth, cannonLength, glm::vec3(0.2, 0.18, 0.13), true));
    meshes["cannon1"] = t1;
    Mesh* prj1 = (object2D::CreateCircle("prj1", glm::vec3(0, 0, 0), projectileCircle_radius, 70, glm::vec3(1, 1, 1), true));
    meshes["prj1"] = prj1;

    Mesh* baseTrapezoid2 = (object2D::CreateTrapezoid("baseTrapezoid2", glm::vec3(0, 0, 0), base_baseTrapezoid, glm::vec3(0.12, 0.35, 0.3), true));
    meshes["baseTrapezoid2"] = baseTrapezoid2;
    Mesh* topTrapezoid2 = (object2D::CreateTrapezoid("topTrapezoid2", glm::vec3(0, 23, 0), -top_baseTrapezoid, glm::vec3(0.01, 0.65, 0.2), true));
    meshes["topTrapezoid2"] = topTrapezoid2;
    Mesh* circle2 = (object2D::CreateCircle("circle2", glm::vec3(0, 23, 0), tankCircle_radius, 70, glm::vec3(0.01, 0.65, 0.2), true));
    meshes["circle2"] = circle2;
    Mesh* t2 = (object2D::CreateRectangle("cannon2", glm::vec3(0, 30, 0), cannonWidth, cannonLength, glm::vec3(0.11, 0.18, 0.13), true));
    meshes["cannon2"] = t2;
    Mesh* prj2 = (object2D::CreateCircle("prj2", glm::vec3(0, 0, 0), projectileCircle_radius, 70, glm::vec3(1, 1, 1), true));
    meshes["prj2"] = prj2;

    Mesh* sun = object2D::CreateCircle("sun", glm::vec3(1000, 600, 0), 50, 100, glm::vec3(1, 1, 0), true); // 500, 700 este poziția pe cer, 50 este raza
    meshes["sun"] = sun;
    
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.36, 0.73, 0.92, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

int Tema1::DetectTerrainCollision(Projectile projectile, std::vector<glm::vec2>& heightMap) {
    float t, f;
    for (int i = 0; i < heightMap.size() - 1; ++i) {
        glm::vec2& A = heightMap[i];
        glm::vec2& B = heightMap[i + 1];

        if (projectile.pos.x >= A.x && projectile.pos.x <= B.x) {
            t = (projectile.pos.x - A.x) / (B.x - A.x);
            f = A.y * (1 - t) + B.y * t;

            if (projectile.pos.y <= f + 5.0f) {
                return 1;
            }
        }
    }
    return 0;
}

void Tema1::DeformTerrain(Projectile projectile, float radius, std::vector<glm::vec2>& heightMap) {
    float d, avgY = 0, nrPoints = 0;
    for (int i = 0; i < heightMap.size(); i++) {
        glm::vec2& A = heightMap[i];
        d = sqrt((A.x - projectile.pos.x) * (A.x - projectile.pos.x) + (A.y - projectile.pos.y) * (A.y - projectile.pos.y));
        if (d <= radius) {
            avgY += A.y;
            nrPoints++;
        }
    }
    avgY /= nrPoints;
    for (int i = 0; i < heightMap.size(); i++) {
        glm::vec2& A = heightMap[i];
        d = sqrt((A.x - projectile.pos.x) * (A.x - projectile.pos.x) + (A.y - projectile.pos.y) * (A.y - projectile.pos.y));
        if (d <= radius) {
            A.y = glm::min(A.y, avgY - sqrt(radius * radius - d * d));
        }
    }
}

void Tema1::TerrainSlide(std::vector<glm::vec2>& heightMap) {
    for (int i = 1; i < heightMap.size() - 1; ++i) {
        glm::vec2& A = heightMap[i - 1];
        glm::vec2& B = heightMap[i];
        glm::vec2& C = heightMap[i+1];

        heightMap[i].y = A.y * 0.05 + B.y * 0.9 + C.y * 0.05;
    }
}

void Tema1::HealthBar(Tank tank, float maxHealth, float currHealth, glm::vec3 color) {
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(tank.x, tank.y + 70);

    Mesh* healthWire = object2D::CreateRectangle("healthWire", glm::vec3(0, 0, 0), barWidth + 5, barHeight + 5, glm::vec3(1, 1, 1), false);
    meshes["healthWire"] = healthWire;
    Mesh* healthFilled = object2D::CreateRectangle("healthFilled", glm::vec3(0, 0, 0), barWidth, barHeight, color, true);
    meshes["healthFilled"] = healthFilled;

    RenderMesh2D(meshes["healthWire"], shaders["VertexColor"], modelMatrix);

    modelMatrix *= transform2D::Translate(-barWidth / 2, 0);
    modelMatrix *= transform2D::Scale(currHealth / maxHealth, 1);
    modelMatrix *= transform2D::Translate(barWidth / 2, 0);
    RenderMesh2D(meshes["healthFilled"], shaders["VertexColor"], modelMatrix);
}

int Tema1::DetectTankCollision(Projectile& projectile, Tank& tank) {
    if (tank.is_alive == true) {
        float dx, dy, d;
        dx = projectile.pos.x - tank.x;
        dy = projectile.pos.y - tank.y;
        d = sqrt(dx * dx + dy * dy);

        if (d <= tank.radius + projectile.radius)
            return 1;
    }
    return 0;
}

void Tema1::CreateTrajectoryLine(const std::string& name, glm::vec2 start, glm::vec2 velocity, float angle, int nrPoints, float magnitude, float g, glm::vec3 color, bool fill, float deltaTime)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    glm::vec2 currPos = start;

    for (int i = 0; i < nrPoints; i++) {
        vertices.emplace_back(glm::vec3(currPos.x, currPos.y, 0), color);
        indices.push_back(i);

        Projectile p;
        p.pos = currPos;
        if (DetectTerrainCollision(p, heightMap)) {
            break;
        }

        currPos += velocity * deltaTime;
        velocity.y -= g * deltaTime;
    }

    Mesh* line = new Mesh(name);

    line->SetDrawMode(GL_LINE_STRIP);

    line->InitFromData(vertices, indices);

    RenderMesh2D(line, shaders["VertexColor"], glm::mat3(1));
}


void Tema1::Update(float deltaTimeSeconds)
{
    glClearColor(0.36, 0.73, 0.92, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    RenderMesh2D(meshes["sun"], shaders["VertexColor"], modelMatrix);

    rotationAngleSun += 0.1 * deltaTimeSeconds;

    glm::mat3 modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["sun"], shaders["VertexColor"], modelMatrix);

    for (int i = 0; i < nrRays; ++i) {
        float angle = (i * 2.0f * M_PI) / nrRays;
        float rotatedAngle = angle + rotationAngleSun;
        glm::vec3 startPoint(1000, 600, 0);
        glm::vec3 endPoint = startPoint + glm::vec3(rayLength * cos(rotatedAngle), rayLength * sin(rotatedAngle), 0);

        Mesh* ray = object2D::CreateLine("ray" + std::to_string(i), startPoint, endPoint, glm::vec3(1, 1, 0));
        meshes["ray" + std::to_string(i)] = ray;
        RenderMesh2D(ray, shaders["VertexColor"], glm::mat3(1));
    }

    for (int i = 0; i < heightMap.size() - 1; ++i) {
        glm::vec2& A = heightMap[i];
        glm::vec2& B = heightMap[i + 1];
        A.y = max(A.y, 0.0f);
        B.y = max(B.y, 0.0f);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(A.x, A.y);
        modelMatrix *= transform2D::Shear((B.y - A.y) / (B.x - A.x), 0);
        modelMatrix *= transform2D::Scale(B.x - A.x, max(B.y, A.y));

        RenderMesh2D(meshes["square_" + std::to_string(i)], shaders["VertexColor"], modelMatrix);
    }

    tank1.x = glm::clamp(tank1.x, 0.0f, (float)resolution.x);
    tank1.y = glm::clamp(tank1.y, 0.0f, (float)resolution.y);
    tank2.x = glm::clamp(tank2.x, 0.0f, (float)resolution.x);
    tank2.y = glm::clamp(tank2.y, 0.0f, (float)resolution.y);

    if (tank1.is_alive == true) {
        glm::vec2& A = heightMap[tank1.x];
        glm::vec2& B = heightMap[tank1.x + 1];

        float t1 = (tank1.x - A.x) / (B.x - A.x);
        tank1.y = A.y + t1 * (B.y - A.y);
        tank1.angle = atan2(B.y - A.y, B.x - A.x);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(tank1.x, tank1.y);
        modelMatrix *= transform2D::Rotate(tank1.angle);

        RenderMesh2D(meshes["baseTrapezoid1"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["topTrapezoid1"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["circle1"], shaders["VertexColor"], modelMatrix);

        glm::vec2 pivot = glm::vec2(0, 30);
        modelMatrix *= transform2D::Translate(pivot.x, pivot.y);
        modelMatrix *= transform2D::Rotate(cannon1.angle);
        modelMatrix *= transform2D::Translate(-pivot.x, -pivot.y);
        RenderMesh2D(meshes["cannon1"], shaders["VertexColor"], modelMatrix);

        HealthBar(tank1, maxHealth, tank1.health, tank1.healthColor);

        modelMatrix *= transform2D::Translate(0, cannonLength);

        initX1 = modelMatrix[2].x;
        initY1 = modelMatrix[2].y;
        glm::vec3 startTrajectory1 = glm::vec3(initX1, initY1, 0);
        glm::vec3 velocityTrajectory1 = glm::vec3(cos(tank1.angle + cannon1.angle + M_PI / 2), sin(tank1.angle + cannon1.angle + M_PI / 2), 0) * magnitude;
        CreateTrajectoryLine("trajectory1", startTrajectory1, velocityTrajectory1, tank1.angle + cannon1.angle + M_PI / 2, 500, magnitude, g, glm::vec3(1, 1, 1), false, deltaTimeSeconds);

        for (int i = 0; i < projectiles1.size(); i++) {
            if (projectiles1[i].is_active == true) {

                projectiles1[i].pos += projectiles1[i].velocity * deltaTimeSeconds;
                projectiles1[i].velocity.y -= g * deltaTimeSeconds;

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectiles1[i].pos.x, projectiles1[i].pos.y);
                RenderMesh2D(meshes["prj1"], shaders["VertexColor"], modelMatrix);

                if (DetectTankCollision(projectiles1[i], tank2) == 1) {
                    tank2.health -= projectiles1[i].damage;
                    tank2.healthColor.x += 0.1;
                    tank2.healthColor.y -= 0.1;
                    tank2.healthColor.z += 0.05;
                    projectiles1[i].is_active = false;
                    if (tank2.health <= 0)
                        tank2.is_alive = false;
                    break;
                } else if (DetectTankCollision(projectiles1[i], tank1) == 1) {
                    tank1.health -= projectiles1[i].damage;
                    tank1.healthColor.x += 0.1;
                    tank1.healthColor.y -= 0.1;
                    tank1.healthColor.z += 0.05;
                    projectiles1[i].is_active = false;
                    if (tank1.health <= 0)
                        tank1.is_alive = false;
                    break;
                } else if (DetectTerrainCollision(projectiles1[i], heightMap) == 1) {
                    DeformTerrain(projectiles1[i], deformRadius, heightMap);
                    projectiles1[i].is_active = false;
                }
                else if (projectiles1[i].pos.y < 0 || projectiles1[i].pos.x < 0 || projectiles1[i].pos.x > resolution.x) {
                    projectiles1[i].is_active = false;
                }
            }
        }
        for (int i = 0; i < projectiles1.size(); i++) {
            if (!projectiles1[i].is_active) {
                projectiles1.erase(projectiles1.begin() + i);
                --i;
            }
        }
    }

    if (tank2.is_alive == true) {
        glm::vec2& C = heightMap[tank2.x];
        glm::vec2& D = heightMap[tank2.x + 1];

        float t2 = (tank2.x - C.x) / (D.x - C.x);
        tank2.y = C.y + t2 * (D.y - C.y);
        tank2.angle = atan2(D.y - C.y, D.x - C.x);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(tank2.x, tank2.y);
        modelMatrix *= transform2D::Rotate(tank2.angle);

        RenderMesh2D(meshes["baseTrapezoid2"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["topTrapezoid2"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["circle2"], shaders["VertexColor"], modelMatrix);

        glm::vec2 pivot = glm::vec2(0, 30);
        modelMatrix *= transform2D::Translate(pivot.x, pivot.y);
        modelMatrix *= transform2D::Rotate(cannon2.angle);
        modelMatrix *= transform2D::Translate(-pivot.x, -pivot.y);
        RenderMesh2D(meshes["cannon2"], shaders["VertexColor"], modelMatrix);

        HealthBar(tank2, maxHealth, tank2.health, tank2.healthColor);

        modelMatrix *= transform2D::Translate(0, cannonLength);

        initX2 = modelMatrix[2].x;
        initY2 = modelMatrix[2].y;
        glm::vec3 startTrajectory2 = glm::vec3(initX2, initY2, 0);
        glm::vec3 velocityTrajectory2 = glm::vec3(cos(tank2.angle + cannon2.angle + M_PI / 2), sin(tank2.angle + cannon2.angle + M_PI / 2), 0) * magnitude;
        CreateTrajectoryLine("trajectory2", startTrajectory2, velocityTrajectory2, tank2.angle + cannon2.angle + M_PI / 2, 500, magnitude, g, glm::vec3(1, 1, 1), false, deltaTimeSeconds);

        for (int i = 0; i < projectiles2.size(); i++) {
            if (projectiles2[i].is_active == true) {

                projectiles2[i].pos += projectiles2[i].velocity * deltaTimeSeconds;
                projectiles2[i].velocity.y -= g * deltaTimeSeconds;

                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(projectiles2[i].pos.x, projectiles2[i].pos.y);
                RenderMesh2D(meshes["prj2"], shaders["VertexColor"], modelMatrix);

                if (DetectTankCollision(projectiles2[i], tank1) == 1) {
                    tank1.health -= projectiles2[i].damage;
                    tank1.healthColor.x += 0.1;
                    tank1.healthColor.y -= 0.1;
                    tank1.healthColor.z += 0.05;
                    projectiles2[i].is_active = false;
                    if (tank1.health <= 0)
                        tank1.is_alive = false;
                    break; 
                } else if (DetectTankCollision(projectiles2[i], tank2) == 1) {
                    tank2.health -= projectiles2[i].damage;
                    tank2.healthColor.x += 0.1;
                    tank2.healthColor.y -= 0.1;
                    tank2.healthColor.z += 0.05;
                    projectiles2[i].is_active = false;
                    if (tank2.health <= 0)
                        tank2.is_alive = false;
                    break;
                }
                else if (DetectTerrainCollision(projectiles2[i], heightMap) == 1) {
                    DeformTerrain(projectiles2[i], deformRadius, heightMap);
                    projectiles2[i].is_active = false;
                }
                else if (projectiles2[i].pos.y < 0 || projectiles2[i].pos.x < 0 || projectiles2[i].pos.x > resolution.x) {
                    projectiles2[i].is_active = false;
                }
            }
        }
        for (int i = 0; i < projectiles2.size(); i++) {
            if (!projectiles2[i].is_active) {
                projectiles2.erase(projectiles2.begin() + i);
                --i;
            }
        }
    }

    TerrainSlide(heightMap);

}

void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_A)) tank1.x -= 50 * deltaTime;
    if (window->KeyHold(GLFW_KEY_D)) tank1.x += 50 * deltaTime;
    if (window->KeyHold(GLFW_KEY_LEFT)) tank2.x -= 50 * deltaTime;
    if (window->KeyHold(GLFW_KEY_RIGHT)) tank2.x += 50 * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        cannon1.angle += deltaTime;
        if (cannon1.angle > M_PI / 2)
            cannon1.angle = M_PI / 2;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        cannon1.angle -= deltaTime;
        if (cannon1.angle < - M_PI / 2)
            cannon1.angle = -M_PI / 2;
    }
    if (window->KeyHold(GLFW_KEY_UP)) {
        cannon2.angle += deltaTime;
        if (cannon2.angle > M_PI / 2)
            cannon2.angle = M_PI / 2;
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        cannon2.angle -= deltaTime;
        if (cannon2.angle < -M_PI / 2)
            cannon2.angle = -M_PI / 2;
    }

    cannon1.pos = glm::vec2(tankCircle_radius * cos(cannon1.angle), tankCircle_radius * sin(cannon1.angle));
    cannon2.pos = glm::vec2(tankCircle_radius * cos(cannon2.angle), tankCircle_radius * sin(cannon2.angle));
}


void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE) {
        projectile1.pos = glm::vec2(initX1, initY1);

        projectile1.velocity = glm::vec2(cos(tank1.angle + cannon1.angle + M_PI / 2), sin(tank1.angle + cannon1.angle + M_PI / 2)) * magnitude;
        projectile1.is_active = true;
        projectiles1.push_back(projectile1);
    }
    if (key == GLFW_KEY_ENTER) {
        projectile2.pos = glm::vec2(initX2, initY2);

        projectile2.velocity = glm::vec2(cos(tank2.angle + cannon2.angle + M_PI / 2), sin(tank2.angle + cannon2.angle + M_PI / 2)) * magnitude;
        projectile2.is_active = true;
        projectiles2.push_back(projectile2);
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
