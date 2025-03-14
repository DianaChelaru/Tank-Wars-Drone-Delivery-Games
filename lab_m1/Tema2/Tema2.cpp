#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/transform3D.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    left = .01f;
    right = 10.0f;
    bottom = .01f;
    top = 10.0f;
    zNear = 0.01f;
    zFar = 200.0f;
    fov = 40.0f;
    perspective = true;

    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    translateX = 0;
    translateY = 0;
    translateZ = 0;

    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    eliceRotation = 0;
    numTrees = 100;

    terrain_color1 = glm::vec3(0.96, 0.96, 0.96);
    terrain_color2 = glm::vec3(0.8, 0.8, 1);
    drone_bodyColor = glm::vec3(0.7, 0.7, 0.7);
    drone_elliceColor = glm::vec3(0, 0, 0);
    tree_trunkColor = glm::vec3(0.5,0.3,0.04);
    tree_leavesColor1 = glm::vec3(0.11, 0.56, 0.11);
    tree_leavesColor2 = glm::vec3(0.9, 0.9, 1);
    package_color = glm::vec3(0.78,0.58,0.01);
    dest_color = glm::vec3(1, 0, 1);
    arrow_color = glm::vec3(1, 0, 1);

    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
    minimapCamera = new implemented::Camera();
    minimapCamera->Set(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    dronePosition = glm::vec3(0, 0, 0);
    life = 5;
    gotHit = false;
    destinationActive = false;
    packageAttached = false;
    packages = 0;
    pos = glm::vec3(0.0f, 0.0f, 0.0f);
    over = false;
    stop_generating = false;

    Pposition = false;
    do {
        packagePosition.x = rand() % 201 - 100;
        packagePosition.z = rand() % 201 - 100;
        packagePosition.y = 1.1;
        float dist = glm::distance(glm::vec2(packagePosition.x, packagePosition.z), glm::vec2(dronePosition.x, dronePosition.z));
        if (dist > 5)
            Pposition = true;
    } while (!Pposition);

    Dposition = false;
    do {
        deliveryPosition.x = rand() % 201 - 100;
        deliveryPosition.z = rand() % 201 - 100;
        deliveryPosition.y = 1.3;
        float dist1 = glm::distance(glm::vec2(packagePosition.x, packagePosition.z), glm::vec2(deliveryPosition.x, deliveryPosition.z));
        float dist2 = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(deliveryPosition.x, deliveryPosition.z));
        if (dist1 > 5 && dist2 > 5)
            Dposition = true;
    } while (!Dposition);

    packagePickedUp = false;

    for (int i = 0; i < numTrees; ++i) {
        float x = rand() % 201 - 100;
        float z = rand() % 201 - 100;
        float distPacket = glm::distance(glm::vec2(packagePosition.x, packagePosition.z), glm::vec2(x, z));
        float distDelivery = glm::distance(glm::vec2(deliveryPosition.x, deliveryPosition.z), glm::vec2(x, z));
        float distDrone = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(x, z));
        if (distPacket < 5 && distDelivery < 5 && distDrone < 5) {
            --i;
        }
        else {
            treePositions.emplace_back(glm::vec3(x, 0, z));
        }
    }

    shader = new Shader("LabShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("noGrassShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "noGrassVertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "noGrassFragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    int m = 50, n = 50;
    float size = 200;
    meshes["terrain"] = CreateTerrain(m, n, size);

    meshes["cone"] = CreateCone(1.2, 2, 30);
    meshes["cylinder"] = CreateCylinder(0.8f, 1.0f, 100);
    meshes["cicle"] = CreateCircle(0.5, 50, glm::vec3(1,0,0));

    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.2)),
            VertexFormat(glm::vec3(1, -1,  1), glm::vec3(1, 0, 1), glm::vec3(0.9, 0.4, 0.2)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 0, 0), glm::vec3(0.7, 0.7, 0.1)),
            VertexFormat(glm::vec3(1,  1,  1), glm::vec3(0, 1, 0), glm::vec3(0.7, 0.3, 0.7)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(0.3, 0.5, 0.4)),
            VertexFormat(glm::vec3(1, -1, -1), glm::vec3(0, 1, 1), glm::vec3(0.5, 0.2, 0.9)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 0), glm::vec3(0.7, 0.0, 0.7)),
            VertexFormat(glm::vec3(1,  1, -1), glm::vec3(0, 0, 1), glm::vec3(0.1, 0.5, 0.8)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,        1, 3, 2,
            2, 3, 7,        2, 7, 6,
            1, 7, 3,        1, 5, 7,
            6, 7, 4,        7, 5, 4,
            0, 4, 1,        1, 4, 5,
            2, 6, 4,        0, 2, 4,
        };
        CreateMesh("cube", vertices, indices);
    }

    textRenderer = new gfxc::TextRenderer(window->props.selfDir, window->GetResolution().x, window->GetResolution().y);
    textRenderer->Load(PATH_JOIN(RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);
}

void Tema2::FrameStart()
{
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema2::RenderColors(Shader* shader, int object) {
    glUseProgram(shader->GetProgramID());
    GLint choose_color = glGetUniformLocation(shader->program, "choose_color");
    glUniform1i(choose_color, object);
    if (object == 0) {
        GLint Tcolor1 = glGetUniformLocation(shader->program, "terrain_color1");
        GLint Tcolor2 = glGetUniformLocation(shader->program, "terrain_color2");
        glUniform3fv(Tcolor1, 1, glm::value_ptr(terrain_color1));
        glUniform3fv(Tcolor2, 1, glm::value_ptr(terrain_color2));
    }
    if (object == 1) {
        GLint DBcolor = glGetUniformLocation(shader->program, "drone_bodyColor");
        glUniform3fv(DBcolor, 1, glm::value_ptr(drone_bodyColor));
    }
    if (object == 2) {
        GLint DEcolor = glGetUniformLocation(shader->program, "drone_elliceColor");
        glUniform3fv(DEcolor, 1, glm::value_ptr(drone_elliceColor));
    }
    if (object == 3) {
        GLint TTcolor = glGetUniformLocation(shader->program, "tree_trunkColor");
        glUniform3fv(TTcolor, 1, glm::value_ptr(tree_trunkColor));
    }
    if (object == 4) {
        GLint TLcolor1 = glGetUniformLocation(shader->program, "tree_leavesColor1");
        glUniform3fv(TLcolor1, 1, glm::value_ptr(tree_leavesColor1));
    }
    if (object == 5) {
        GLint Pcolor = glGetUniformLocation(shader->program, "package_color");
        glUniform3fv(Pcolor, 1, glm::value_ptr(package_color));
    }
    if (object == 6) {
        GLint Destcolor = glGetUniformLocation(shader->program, "dest_color");
        glUniform3fv(Destcolor, 1, glm::value_ptr(dest_color));
    }
    if (object == 7) {
        GLint Arrowcolor = glGetUniformLocation(shader->program, "arrow_color");
        glUniform3fv(Arrowcolor, 1, glm::value_ptr(arrow_color));
    }
    if(object == 8) {
        GLint TLcolor2 = glGetUniformLocation(shader->program, "tree_leavesColor2");
        glUniform3fv(TLcolor2, 1, glm::value_ptr(tree_leavesColor2));
    }
}

bool Tema2::CheckCollisionWithPackage(glm::vec3 dronePosition, float droneRadius, glm::vec3 packagePosition, float packageRadius) {
    float distanceXZ = glm::distance(glm::vec2(dronePosition.x, dronePosition.z), glm::vec2(packagePosition.x, packagePosition.z));
    float distanceY = abs(dronePosition.y - packagePosition.y);
    float heightThreshold = 1.7;

    return (distanceXZ < (droneRadius + packageRadius) && distanceY <= heightThreshold);
}

bool Tema2::CheckCollisionWithDestination(glm::vec3 packagePosition, float packageRadius, glm::vec3 deliveryPosition, float deliveryRadius) {
    float distanceXZ = glm::distance(glm::vec2(packagePosition.x, packagePosition.z), glm::vec2(deliveryPosition.x, deliveryPosition.z));
    float distanceY = abs(packagePosition.y - deliveryPosition.y);
    float heightThreshold = 0.8f;
    return (distanceXZ < (packageRadius + deliveryRadius) && distanceY <= heightThreshold);
}

void Tema2::RenderMainScene(float deltaTimeSeconds, bool over) 
{
    if (life > 0) {
        std::string lifeText = "Life Left: " + std::to_string((int)life);
        textRenderer->RenderText(lifeText, 10, 50, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f,0.2));
    }

    if (life <= 0 || (packages < numPackages && timer <= 0)) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::ivec2 resolution = window->GetResolution();
        float posX = (resolution.x-350) / 2.0f;
        float posY = (resolution.y-50) / 2.0f;
        if (over == true) {
            std::string text = "GAME OVER";
            textRenderer->RenderText(text, posX, posY, 3, glm::vec3(1,0,0));
        }
        return;
    }

    if (packages <= numPackages) {
        std::string timerText = "Time Left: " + std::to_string((int)timer) + "s";
        textRenderer->RenderText(timerText, 10, 10, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        std::string text = "Collected packages: " + std::to_string((int)packages);
        textRenderer->RenderText(text, 950, 10, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    if (packages == numPackages && time > 0) {
        stop_generating = true;
        glm::ivec2 resolution = window->GetResolution();
        float posX = (resolution.x - 350) / 2.0f;
        float posY = (resolution.y - 50) / 2.0f;
        if (over == true) {
            std::string text = "GOOD JOB";
            textRenderer->RenderText(text, posX, posY, 4, glm::vec3(1, 1, 1));
        }
    }

    eliceRotation += 5.0f * deltaTimeSeconds;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOY(glm::radians(45.0f));
    modelMatrix *= transform3D::Scale(1.0f, 0.075f, 0.05f);
    RenderColors(shaders["noGrassShader"], 1);
    if (mini == 0)
        RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
    else
        RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);

    dronePosition = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOY(glm::radians(-45.0f));
    modelMatrix *= transform3D::Scale(1.0f, 0.075f, 0.05f);
    RenderColors(shaders["noGrassShader"], 1);
    if (mini == 0)
        RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
    else
        RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);

    for (int i = 0; i < 4; i++) {
        float angle = glm::radians(45.0f + i * 90.0f);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix *= transform3D::Translate(cos(angle) * 0.93, 0.1f, sin(angle) * 0.93);
        modelMatrix *= transform3D::RotateOY(angle);
        modelMatrix *= transform3D::Scale(0.05, 0.075, 0.075);
        RenderColors(shaders["noGrassShader"], 1);
        if (mini == 0)
            RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
        else
            RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix *= transform3D::Translate(cos(angle) * 0.93, 0.18f, sin(angle) * 0.93);
        modelMatrix *= transform3D::RotateOY(eliceRotation);
        modelMatrix *= transform3D::Scale(0.0085f, 0.009f, 0.25f);
        RenderColors(shaders["noGrassShader"], 2);
        if (mini == 0)
            RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
        else
            RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);
    }

    modelMatrix = glm::mat4(1);
    RenderColors(shaders["LabShader"], 0);
    if (mini == 0)
        RenderMesh(meshes["terrain"], shaders["LabShader"], modelMatrix);
    else
        RenderMeshMiniMap(meshes["terrain"], shaders["LabShader"], modelMatrix);

    for (const auto& treePosition : treePositions)
        RenderTree(treePosition);

    if (!packageAttached && stop_generating == false) {
        if (CheckCollisionWithPackage(dronePosition, droneRadius, packagePosition, packageSize)) {
            packageAttached = true;
            glm::vec3 direction = glm::normalize(dronePosition - packagePosition);
            dronePosition += direction * (droneRadius + packageSize - glm::distance(dronePosition, packagePosition));
        }
        else {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, packagePosition);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(packageSize, packageSize, packageSize));
            RenderColors(shaders["noGrassShader"], 5);
            if (mini == 0)
                RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
            else
                RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);
        }
    }

    if (packageAttached && stop_generating == false) {
        packagePickedUp = true;
        packagePosition = dronePosition - glm::vec3(0.0f,packageSize + 0.07, 0.0f);
        float terrainHeight = GetTerrainHeight(packagePosition.x, packagePosition.z);
        if (packagePosition.y < terrainHeight) {
            packagePosition.y = terrainHeight;
        }

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, packagePosition);
        modelMatrix *= transform3D::RotateOY(angularStepOY);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(packageSize, packageSize, packageSize));
        packagePosition = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

        RenderColors(shaders["noGrassShader"], 5);
        if (mini == 0)
            RenderMesh(meshes["cube"], shaders["noGrassShader"], modelMatrix);
        else
            RenderMeshMiniMap(meshes["cube"], shaders["noGrassShader"], modelMatrix);
    }

    if (packagePickedUp && stop_generating == false) {
        destinationActive = true;
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, deliveryPosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(2,2,2));
        RenderColors(shaders["noGrassShader"], 6);
        if (mini == 0)
            RenderMesh(meshes["circle"], shaders["noGrassShader"], modelMatrix);
        else
            RenderMeshMiniMap(meshes["circle"], shaders["noGrassShader"], modelMatrix);
    }

    if (packageAttached && destinationActive && stop_generating == false) {
        if (CheckCollisionWithDestination(packagePosition, packageSize, deliveryPosition, 0.5f)) {
            packagePickedUp = false;
            cout << "Ai livrat un pachet! <3\n";
            packages++;
            cout << "Pachete livrate pana acum: " << packages << "\n";
            packageAttached = false;
            destinationActive = false;
            packagePosition.x = rand() % 201 - 100;
            packagePosition.z = rand() % 201 - 100;
            packagePosition.y = GetTerrainHeight(packagePosition.x, packagePosition.z) + packageSize;
        }
    }

    float angle;
    if (stop_generating == false) {
        if (!packageAttached) {
            glm::vec3 directionToPackage = glm::normalize(packagePosition - dronePosition);
            angle = atan2(directionToPackage.z, directionToPackage.x);
        }
        else {
            glm::vec3 directionToDest = glm::normalize(deliveryPosition - dronePosition);
            angle = atan2(directionToDest.z, directionToDest.x);
        }

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, dronePosition + glm::vec3(0, 0.5, 0)); // Pozitionat deasupra dronei
        modelMatrix *= transform3D::RotateOY(glm::radians(90.0f) - angle);
        modelMatrix *= transform3D::RotateOX(glm::radians(90.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.6f, 0.2f)); // Scalam conul sa fie mai mic

        RenderColors(shaders["noGrassShader"], 7);
        if (mini == 0)
            RenderMesh(meshes["cone"], shaders["noGrassShader"], modelMatrix);
        else
            RenderMeshMiniMap(meshes["cone"], shaders["noGrassShader"], modelMatrix);
    }
}

void Tema2::Update(float deltaTimeSeconds)
{
    mini = 0;
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    if (timer > 0 && packages != numPackages) {
        timer -= deltaTimeSeconds;
    }

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderMainScene(deltaTimeSeconds, true);
    if (life <= 0 || (packages < numPackages && timer <= 0)) {
        glClear(GL_DEPTH_BUFFER_BIT);
        RenderMainScene(deltaTimeSeconds, true);
        return;
    }
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    minimapCamera->position = dronePosition + glm::vec3(0, 6, 0);
    minimapCamera->forward = glm::normalize(glm::vec3(0, -1, 0));
    minimapCamera->up = glm::vec3(0, 0, -1);
    minimapCamera->right = glm::cross(minimapCamera->forward, minimapCamera->up);
    mini = 1;

    RenderMainScene(deltaTimeSeconds, false);
}

Mesh* Tema2::CreateCircle(float radius, int numSegments,glm::vec3 color) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 1, 0), color);

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * i / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.emplace_back(glm::vec3(x, 0.0f, z), glm::vec3(0, 1, 0), color);
    }

    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* circle = new Mesh("circle");
    circle->InitFromData(vertices, indices);
    meshes[circle->GetMeshID()] = circle;
    return circle;
}


Mesh* Tema2::CreateTerrain(int m, int n, float size) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float step = size / (float)(m);

    for (int i = 0; i < m; ++i) {
        float z = (i * step) - size / 2;
        for (int j = 0; j < n; ++j) {
            float x = (j * step) - size / 2;
            vertices.emplace_back(glm::vec3(x, 0, z));
        }
    }

    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            int topLeft = i * n + j;
            int topRight = topLeft + 1;
            int bottomLeft = (i + 1) * n + j;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    Mesh* terrain = new Mesh("terrain");
    terrain->InitFromData(vertices, indices);
    return terrain;
}

Mesh* Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    // Unbind the VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}

Mesh* Tema2::CreateCylinder(float radius, float height, int slices) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    for (int i = 0; i <= slices; ++i) {
        float theta = (2.0f * M_PI * i) / slices;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), glm::vec3(0.0f, -1.0f, 0.0f)));
        vertices.push_back(VertexFormat(glm::vec3(x, height, z), glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    for (int i = 0; i < slices; ++i) {
        indices.push_back(i * 2);
        indices.push_back(i * 2 + 1);
        indices.push_back(i * 2 + 2);

        indices.push_back(i * 2 + 1);
        indices.push_back(i * 2 + 3);
        indices.push_back(i * 2 + 2);
    }
    Mesh* cylinder = new Mesh("cylinder");
    cylinder->InitFromData(vertices, indices);
    return cylinder;
}

Mesh* Tema2::CreateCone(float radius, float height, int segments) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.emplace_back(glm::vec3(0, 0, 0));

    for (int i = 0, n = segments - 1; i < segments; ++i) {
        float ratio = (float)i / n;
        float angle = ratio * (2 * M_PI);
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        vertices.emplace_back(glm::vec3(x, 0.0f, z));
    }

    vertices.emplace_back(glm::vec3(0, height, 0));

    for (int i = 0; i < segments; ++i) {
        indices.push_back(0);
        indices.push_back(1 + i);
        indices.push_back(1 + (i + 1) % segments);
    }

    int centerIndex = vertices.size() - 1;
    for (int i = 0; i < segments; ++i) {
        indices.push_back(centerIndex);
        indices.push_back(1 + (i + 1) % segments);
        indices.push_back(1 + i);
    }

    Mesh* cone = new Mesh("cone");
    cone->InitFromData(vertices, indices);
    return cone;
}

void Tema2::RenderTree(glm::vec3 position) {
    float terrainHeight = GetTerrainHeight(position.x, position.z);
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, terrainHeight, position.z));
    modelMatrix *= transform3D::Scale(1, 5, 1);
    RenderColors(shaders["noGrassShader"], 3);
    if (mini == 1)
        RenderMeshMiniMap(meshes["cylinder"], shaders["noGrassShader"], modelMatrix);
    else 
        RenderMesh(meshes["cylinder"], shaders["noGrassShader"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, terrainHeight + 4.7, position.z));
    modelMatrix *= transform3D::Scale(2.5,3.2,2.5);
    RenderColors(shaders["noGrassShader"], 4);
    if (mini == 1)
        RenderMeshMiniMap(meshes["cone"], shaders["noGrassShader"], modelMatrix);
    else
        RenderMesh(meshes["cone"], shaders["noGrassShader"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, terrainHeight + 6, position.z));
    modelMatrix *= transform3D::Scale(2.5, 3.2, 2.5);
    RenderColors(shaders["noGrassShader"], 8);
    if (mini == 1)
        RenderMeshMiniMap(meshes["cone"], shaders["noGrassShader"], modelMatrix);
    else
        RenderMesh(meshes["cone"], shaders["noGrassShader"], modelMatrix);
}

void Tema2::FrameEnd()
{
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMeshMiniMap(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(minimapCamera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

float Tema2::Random(glm::vec2 st) {
    return glm::fract(sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float Tema2::Noise(glm::vec2 st) {
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = glm::fract(st);

    float a = Random(i);
    float b = Random(i + glm::vec2(1.0f, 0.0f));
    float c = Random(i + glm::vec2(0.0f, 1.0f));
    float d = Random(i + glm::vec2(1.0f, 1.0f));

    glm::vec2 u = f * f * (3.0f - 2.0f * f);
    return glm::mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
}

float Tema2::GetTerrainHeight(float x, float z) {
    float frequency = 0.3;
    float amplitude = 1;

    glm::vec2 positionXZ(x, z);
    return Noise(positionXZ * frequency) * amplitude;
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W)) {
        pos.z = speed;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        pos.z = -speed;
    }
    if (window->KeyHold(GLFW_KEY_A)) {
        pos.x = -speed;
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        pos.x = speed;
    }
    if (window->KeyHold(GLFW_KEY_Q)) {
        pos.y = speed;
    }
    if (window->KeyHold(GLFW_KEY_E)) {
        pos.y = -speed;
    }

    pos.x *= 0.1;
    pos.y *= 0.1;
    pos.z *= 0.1;

    float terrainHeight = GetTerrainHeight(dronePosition.x, dronePosition.z);

    if (dronePosition.y < terrainHeight) {
        dronePosition.y = terrainHeight;
        pos.y = 1;
    }

    collisionDetected = false;

    for (const auto& treePosition : treePositions) {
        if (CheckCollisionWithCylinder(dronePosition, 1, treePosition, 1, 5)) {
            collisionDetected = true;

            if (gotHit == false) {
                life--;
            }
            gotHit = true;

            glm::vec3 direction = glm::normalize(dronePosition - glm::vec3(treePosition.x, dronePosition.y, treePosition.z));
            glm::vec3 correctedVelocity = pos - glm::dot(pos, glm::normalize(direction)) * direction;

            camera->TranslateRight(correctedVelocity.x * deltaTime);
            camera->TranslateUpward(correctedVelocity.y * deltaTime);
            camera->MoveForward(correctedVelocity.z * deltaTime);
            minimapCamera->TranslateRight(correctedVelocity.x * deltaTime);
            minimapCamera->TranslateUpward(correctedVelocity.y * deltaTime);
            minimapCamera->MoveForward(correctedVelocity.z * deltaTime);
            break;
        }
        else if (CheckCollisionWithCone(dronePosition, 1.2, treePosition + glm::vec3(0, 5, 0), 2.5, 5) ||
            CheckCollisionWithCone(dronePosition, 1.2, treePosition + glm::vec3(0, 6.5, 0), 2.5, 5)) {
            collisionDetected = true;

            if (gotHit == false) {
                life--;
            }
            gotHit = true;

            glm::vec3 direction = glm::normalize(dronePosition - glm::vec3(treePosition.x, dronePosition.y, treePosition.z));
            glm::vec3 correctedVelocity = pos - glm::dot(pos, glm::normalize(direction)) * direction;

            camera->TranslateRight(correctedVelocity.x * deltaTime);
            camera->TranslateUpward(correctedVelocity.y * deltaTime);
            camera->MoveForward(correctedVelocity.z * deltaTime);
            minimapCamera->TranslateRight(correctedVelocity.x * deltaTime);
            minimapCamera->TranslateUpward(correctedVelocity.y * deltaTime);
            minimapCamera->MoveForward(correctedVelocity.z * deltaTime);
            break;
        }
    }

    if (!collisionDetected) {
        gotHit = false;
        camera->TranslateRight(pos.x * deltaTime);
        camera->TranslateUpward(pos.y * deltaTime);
        camera->MoveForward(pos.z * deltaTime);
        minimapCamera->TranslateRight(pos.x * deltaTime);
        minimapCamera->TranslateUpward(pos.y * deltaTime);
        minimapCamera->MoveForward(pos.z * deltaTime);
    }
}

bool Tema2::CheckCollisionWithCylinder(glm::vec3 dronePosition, float droneRadius, glm::vec3 cylinderPosition, float cylinderRadius, float cylinderHeight)
{
    float dx = dronePosition.x - cylinderPosition.x;
    float dz = dronePosition.z - cylinderPosition.z;
    float distance = sqrt(dx * dx + dz * dz);

    if (distance < (cylinderRadius + droneRadius)) {
        if ((dronePosition.y + droneRadius*2) > cylinderPosition.y && dronePosition.y < (cylinderPosition.y + cylinderHeight)) {
            return true;
        }
    }

    return false;
}

bool Tema2::CheckCollisionWithCone(glm::vec3 dronePosition, float droneRadius, glm::vec3 coneBasePosition, float coneBaseRadius, float coneHeight)
{
    float droneY = dronePosition.y;
    float coneBaseY = coneBasePosition.y;
    float coneTopY = coneBaseY + coneHeight;

    //if (droneY < coneBaseY - droneRadius || droneY > coneTopY + droneRadius) {
    //    return false;
    //}

    float normalizedHeight = (droneY - coneBaseY) / coneHeight;
    float coneRadiusAtY = coneBaseRadius * (1.0f - normalizedHeight);

    float dx = dronePosition.x - coneBasePosition.x;
    float dz = dronePosition.z - coneBasePosition.z;
    float distanceXZ = sqrt(dx * dx + dz * dz);

    if (distanceXZ < (coneRadiusAtY + droneRadius)) {
        return true;
    }

    return false;
}

void Tema2::OnKeyPress(int key, int mods)
{

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
            camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
            minimapCamera->RotateThirdPerson_OX(-sensivityOX * deltaY);
            minimapCamera->RotateThirdPerson_OY(-sensivityOY * deltaX);

            angularStepOY -= deltaX * 0.001;

            minimapCamera->right = glm::cross(minimapCamera->forward, minimapCamera->up);
        }
    }

}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}