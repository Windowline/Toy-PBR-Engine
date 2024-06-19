#include "Sphere2.hpp"
#include <glad/glad.h>

#include <vector>

using namespace std;

Sphere2::Sphere2(float radius, vec3 color) : _radius(radius), _color(std::move(color)) {
    constexpr int SECTOR_COUNT = 50;
    constexpr int STACK_COUNT = 50;
    constexpr double PI = 3.14159265358979323846264338327950288;

    float lengthInv = 1.0f / _radius;
    float sectorStep = 2 * PI / SECTOR_COUNT;
    float stackStep = PI / STACK_COUNT;
    float sectorAngle, stackAngle;

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec3> colors;

    for(int i = 0; i <= STACK_COUNT; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        float xy = _radius * cos(stackAngle);
        float z = _radius * sin(stackAngle);

        for(int j = 0; j <= SECTOR_COUNT; ++j) {
            sectorAngle = j * sectorStep;
            float x = xy * cos(sectorAngle);   // r * cos(u) * cos(v)
            float y = xy * sin(sectorAngle);   // r * cos(u) * sin(v)

            positions.emplace_back(vec3(x, y, z));
            normals.emplace_back(vec3(x * lengthInv, y * lengthInv, z * lengthInv));
            colors.emplace_back(_color);
        }
    }


    std::vector<unsigned int> indices;

    for(int i = 0; i < STACK_COUNT; ++i) {
        float k1 = i * (SECTOR_COUNT + 1);     //current stack
        float k2 = k1 + SECTOR_COUNT + 1;      //next stack
        for(int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2) {
            // k1 => k2 => k1+1
            if(i != 0) {
                indices.emplace_back(k1);
                indices.emplace_back(k2);
                indices.emplace_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (STACK_COUNT-1)) {
                indices.emplace_back(k1 + 1);
                indices.emplace_back(k2);
                indices.emplace_back(k2 + 1);
            }
        }
    }

    vector<float> vertices;

    for (int i = 0; i < positions.size(); ++i) {
        auto p = positions[i];
        auto c = colors[i];
        auto n = normals[i];

        for (int i = 0; i < 3; ++i) {
            vertices.push_back(p.x);
            vertices.push_back(p.y);
            vertices.push_back(p.z);

            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);

            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }
    }

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Sphere2::render() const {
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, _indSize, GL_UNSIGNED_INT, 0);
}