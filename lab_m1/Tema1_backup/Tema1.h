#pragma once

#include "components/simple_scene.h"
#include "Tank.h"
#include "Cannon.h"
#include "Projectile.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void CreateTrajectoryLine(const std::string& name, glm::vec2 start, glm::vec2 velocity, float angle, int nrPoints, float magnitude, float g, glm::vec3 color, bool fill, float deltaTime);
        int DetectTerrainCollision(Projectile projectile, std::vector<glm::vec2>& heightMap);
        void DeformTerrain(Projectile projectile, float radius, std::vector<glm::vec2>& heightMap);
        void TerrainSlide(std::vector<glm::vec2>& heightMap);
        void HealthBar(Tank tank, float maxHealth, float currHealth, glm::vec3 color);
        int DetectTankCollision(Projectile& projectile, Tank& tank);

    protected:
        glm::mat3 modelMatrix;

        std::vector<glm::vec2> heightMap;
        int nrPoints = 1000;
        Tank tank1;
        Tank tank2;
        float base_baseTrapezoid = 55;
        float top_baseTrapezoid = 75;
        float tankCircle_radius = 17;
        float cannonLength = 35;
        float cannonWidth = 5;
        Cannon cannon1;
        Cannon cannon2;
        float projectileCircle_radius = 4;
        Projectile projectile1;
        Projectile projectile2;
        std::vector<Projectile> projectiles1;
        std::vector<Projectile> projectiles2;
        float g = 100;
        float magnitude = 150;
        float initX1, initY1;
        float initX2, initY2;
        float deformRadius = 85;
        float barWidth = 70;
        float barHeight = 10;
        float maxHealth = 100;
        int nrRays = 16;
        float rayLength = 80;
        float rotationAngleSun = 0;

    };
}   // namespace m1
