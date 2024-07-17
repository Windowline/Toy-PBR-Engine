#include "Sphere.hpp"
#include <glad/glad.h>

#include <vector>

using namespace std;

Sphere::Sphere(float radius, vec3 color, string name) : _radius(radius), _color(color) {
    _name = name;

    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;
    constexpr float PI = 3.14159265359f;

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    vector<vec3> positions;
    vector<vec3> colors;
    vector<vec3> normals;
    vector<unsigned int> indices;

    positions.reserve((X_SEGMENTS + 1) * (Y_SEGMENTS + 1));
    colors.reserve((X_SEGMENTS + 1) * (Y_SEGMENTS + 1));
    normals.reserve((X_SEGMENTS + 1) * (Y_SEGMENTS + 1));
    indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = cos(ySegment * PI);
            float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

            vec3 p = vec3(xPos, yPos, zPos).normalized() * radius;

            positions.push_back(vec3(p.x, p.y, p.z));
            colors.push_back(_color);
            normals.push_back(vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) { // even rows: y == 0, y == 2; and so on
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    MeshBasic::buildVAO(positions, colors, normals, indices);
}

void Sphere::render(int instanceCount) const {
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if (instanceCount == 1)
        glDrawElements(GL_TRIANGLE_STRIP, _indSize, GL_UNSIGNED_INT, 0);
    else
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, _indSize, GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);
}