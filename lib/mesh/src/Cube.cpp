#include "Cube.hpp"
#include <glad/glad.h>

Cube::Cube(int size, vec3 color, string name) : _size(size), _color(color) {
    _name = name;
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

    MeshBasic::buildVAO(positions, colors, normals, indices);
}