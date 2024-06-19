#include "Room2.hpp"
#include <glad/glad.h>

using namespace std;

Room2::Room2(float size, vec3 backColor, vec3 topColor, vec3 leftColor,
           vec3 rightColor, vec3 bottomColor)

        :   _size(size),
            _backColor(std::move(backColor)),
            _topColor(std::move(topColor)),
            _leftColor(std::move(leftColor)),
            _rightColor(std::move(rightColor)),
            _bottomColor(std::move(bottomColor))
{
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


    //내부가 보여야 하므로 CW 방향으로 구성합니다.
    std::vector <unsigned int> indices = {
            //   0,  1,  2,      0,  2,  3,    // front
            6,  5,  4,      7,  6,  4,    // back
            10,  9,  8,     11,  10, 8,   // top
            14, 13, 12,     15, 14, 12,   // bottom
            18, 17, 16,     19, 18, 16,   // right
            22, 21, 20,     23, 22, 20,   // left
    };

    _indSize = indices.size();

    std::vector<vec3> colors;
    colors.reserve(positions.size());

    std::vector<vec3> normals;
    normals.reserve(positions.size());

    //front
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(vec3(0, 0, 0));
        normals.emplace_back(vec3(0, 0, 1));
    }

    //back
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(_backColor);
        normals.emplace_back(vec3(0, 0, 1));
    }

    //top
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(_topColor);
        normals.emplace_back(vec3(0, -1, 0));
    }

    //bottom
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(_bottomColor);
        normals.emplace_back(vec3(0, 1, 0));
    }

    //right
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(_rightColor);
        normals.emplace_back(vec3(-1, 0, 0));
    }

    //left
    for (int i = 0; i < 4; ++i) {
        colors.emplace_back(_leftColor);
        normals.emplace_back(vec3(1, 0, 0));
    }

    vector<float> vertices;

    for (int i = 0; i < positions.size(); ++i) {
        auto p = positions[i];
        auto c = colors[i];
        auto n = normals[i];

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

void Room2::render() const {
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, _indSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
