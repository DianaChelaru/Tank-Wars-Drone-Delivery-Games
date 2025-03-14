#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderMainScene(float deltaTimeSeconds, bool over);
        Mesh* CreateTerrain(int m, int n, float size);
        Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void RenderColors(Shader* shader, int object);
        Mesh* CreateCylinder(float radius, float height, int slices);
        Mesh* CreateCone(float radius, float height, int segments);
        void RenderTree(glm::vec3 position);
        float GetTerrainHeight(float x, float z);
        bool CheckCollisionWithCylinder(glm::vec3 dronePosition, float droneRadius, glm::vec3 cylinderPosition, float cylinderRadius, float cylinderHeight);
        bool CheckCollisionWithCone(glm::vec3 dronePosition, float droneRadius, glm::vec3 coneBasePosition, float coneBaseRadius, float coneHeight);
        float Random(glm::vec2 st);
        float Noise(glm::vec2 st);
        bool CheckCollisionWithPackage(glm::vec3 dronePosition, float droneRadius, glm::vec3 packagePosition, float packageRadius);
        bool CheckCollisionWithDestination(glm::vec3 packagePosition, float packageRadius, glm::vec3 deliveryPosition, float deliveryRadius);
        Mesh* CreateCircle(float radius, int numSegments, glm::vec3 color);
        void RenderMeshMiniMap(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);

    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        float left, right, bottom, top, zNear, zFar, fov;
        bool perspective;

        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        GLenum polygonMode;
        ViewportArea miniViewportArea;
        float eliceRotation;
        glm::vec3 terrain_color1;
        glm::vec3 terrain_color2;
        glm::vec3 drone_bodyColor;
        glm::vec3 drone_elliceColor;
        int numTrees;
        glm::vec3 tree_trunkColor;
        glm::vec3 tree_leavesColor1;
        glm::vec3 tree_leavesColor2;
        std::vector<glm::vec3> treePositions;
        glm::vec3 dronePosition;
        glm::vec3 pos;
        float droneRadius = 1;
        glm::vec3 packagePosition;
        float packageSize = 0.5;
        glm::vec3 package_color;
        bool packageAttached;
        glm::vec3 deliveryPosition;
        bool destinationActive;
        float deliveryRadius = 3;
        glm::vec3 dest_color;
        float terrainSize = 150;
        bool Dposition;
        bool Pposition;
        int packages;
        Shader* shader;
        implemented::Camera* minimapCamera;
        int mini;
        bool packagePickedUp;
        glm::vec3 arrow_color;
        float life;
        bool gotHit;
        bool over;
        int numPackages = 1;
        bool stop_generating;
        float timer = 60;
        bool collisionDetected;
        float speed = 100;
        gfxc::TextRenderer* textRenderer;
    };
}   // namespace m1