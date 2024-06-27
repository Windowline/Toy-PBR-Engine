#include "Room.hpp"
#include <glad/glad.h>

using namespace std;

Room::Room(float size, vec3 backColor, vec3 topColor, vec3 leftColor,
           vec3 rightColor, vec3 bottomColor, string name)
        :   _size(size),
            _backColor(std::move(backColor)),
            _topColor(std::move(topColor)),
            _leftColor(std::move(leftColor)),
            _rightColor(std::move(rightColor)),
            _bottomColor(std::move(bottomColor))
{
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


    //내부가 보여야 하므로 CW 방향으로 구성합니다.
    std::vector <unsigned int> indices = {
            //   0,  1,  2,      0,  2,  3,    // front
            6,  5,  4,      7,  6,  4,    // back
            10,  9,  8,     11,  10, 8,   // top
            14, 13, 12,     15, 14, 12,   // bottom
            18, 17, 16,     19, 18, 16,   // right
            22, 21, 20,     23, 22, 20,   // left
    };

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

    MeshBasic::buildVAO(positions, colors, normals, indices);
}