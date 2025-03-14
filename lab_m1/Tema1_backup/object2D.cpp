#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(0, -length, 0), color),
        VertexFormat(corner + glm::vec3(length, -length, 0), color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateTrapezoid(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    float bottom = length * 0.7;
    float height = length / 5.5;
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(- bottom / 2, 0, 0), color),
        VertexFormat(corner + glm::vec3(bottom / 2, 0, 0), color),
        VertexFormat(corner + glm::vec3(bottom / 2 + (length - bottom) / 2, height, 0), color),
        VertexFormat(corner + glm::vec3(- (bottom / 2 + (length - bottom) / 2), height, 0), color)
    };

    Mesh* trapezoid = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 0, 2 };

    if (!fill) {
        trapezoid->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    trapezoid->InitFromData(vertices, indices);
    return trapezoid;
}

Mesh* object2D::CreateCircle(
    const std::string& name,
    glm::vec3 center,
    float radius,
    int nrSegments,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    float angle, x_polar, y_polar;

    vertices.emplace_back(center, color);

    for (int i = 0; i <= nrSegments; i++) {
        angle = i * 2 * M_PI / nrSegments;
        x_polar = center.x + radius * cos(angle);
        y_polar = center.y + radius * sin(angle);
        glm::vec3 point = glm::vec3(x_polar, y_polar, 0);
        vertices.emplace_back(point, color);
        indices.push_back(0);
        indices.push_back(i);
        if (i == nrSegments)
            indices.push_back(1);
        else
            indices.push_back(i + 2);
    }

    Mesh* circle = new Mesh(name);

    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* object2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float l,
    float L,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(- l / 2, 0, 0), color),
        VertexFormat(corner + glm::vec3(l / 2, 0, 0), color),
        VertexFormat(corner + glm::vec3(l / 2, L, 0), color),
        VertexFormat(corner + glm::vec3(- l / 2, L, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateLine(const std::string& name, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, bool fill) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(start, color),
        VertexFormat(end, color)
    };

    std::vector<unsigned int> indices = { 0, 1 };

    Mesh* line = new Mesh(name);
    line->InitFromData(vertices, indices);
    line->SetDrawMode(GL_LINES);

    return line;
}
