#include "Cube2.hpp"
#include <glad/glad.h>

Cube2::Cube2(int size, vec3 color) : _VAO(-1), _VBO(-1), _EBO(-1), _size(size), _color(color) {
    float hSize = _size / 2.f;

    std::vector<vec3> positions = {
        // Front face
        vec3(-hSize, -hSize, hSize),
        vec3(hSize, -hSize,  hSize),
        vec3(hSize,  hSize,  hSize),
        vec3(-hSize,  hSize, hSize),

        // Back face
        vec3(-hSize, -hSize, -hSize),
        vec3(-hSize,  hSize, -hSize),
        vec3(hSize,  hSize, -hSize),
        vec3(hSize, -hSize, -hSize),

        // Top face
        vec3(-hSize, hSize, -hSize),
        vec3(-hSize,  hSize,  hSize),
        vec3(hSize,  hSize,  hSize),
        vec3(hSize,  hSize, -hSize),

        // Bottom face
        vec3(-hSize, -hSize, -hSize),
        vec3(hSize, -hSize, -hSize),
        vec3(hSize, -hSize,  hSize),
        vec3(-hSize, -hSize,  hSize),

        // Right face
        vec3(hSize, -hSize, -hSize),
        vec3(hSize,  hSize, -hSize),
        vec3(hSize,  hSize,  hSize),
        vec3(hSize, -hSize,  hSize),

        // Left face
        vec3(-hSize, -hSize, -hSize),
        vec3(-hSize, -hSize,  hSize),
        vec3(-hSize,  hSize,  hSize),
        vec3(-hSize,  hSize, -hSize),
    };

    std::vector<vec3> colors;
    colors.reserve(positions.size());

    for (int i = 0; i < positions.size(); ++i) {
        colors.emplace_back(_color);
    }

    std::vector<vec3> normals;

    //front
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(0, 0, 1));
    //back
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(0, 0, -1));
    //top
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(0, 1, 0));
    //bottom
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(0, -1, 0));
    //right
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(1, 0, 0));
    //left
    for (int i = 0; i < 4; ++i)
        normals.emplace_back(vec3(-1, 0, 0));


    std::vector<unsigned int> indices = {
      0,  1,  2,      0,  2,  3,    // front
      4,  5,  6,      4,  6,  7,    // back
      8,  9,  10,     8,  10, 11,   // top
      12, 13, 14,     12, 14, 15,   // bottom
      16, 17, 18,     16, 18, 19,   // right
      20, 21, 22,     20, 22, 23,   // left
    };

    _indSize = indices.size();

    //create vao
    std::vector<float> vertices;

    for (int i = 0; i < positions.size(); ++i) {
        auto position = positions[i];
        auto color = colors[i];
        auto normal = normals[i];

        vertices.push_back(position.x);
        vertices.push_back(position.y);
        vertices.push_back(position.z);

        vertices.push_back(color.x);
        vertices.push_back(color.y);
        vertices.push_back(color.z);

        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);
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

void Cube2::render() const {
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _indSize, GL_UNSIGNED_INT, 0);
}