//
//  LongCube.cpp
//  RenderTest
//
//  Created by 박창선 on 2022/12/31.
//

#include "../include/LongCube.hpp"

LongCube::LongCube(float size, vec3 color) : _size(std::move(size)), _color(std::move(color)) {
    
    buildMesh();
    _rayCheckType = Mesh::RayCheckType::Box;
}

void LongCube::buildMesh() {
    
    float hSize = _size / 2.f;
    
    std::vector<vec3> vertices = {
        vec3(-0.5 * hSize,  0.0,         0.0),
        vec3(0.5 * hSize,   0.0,         0.0),
        vec3(-0.5 * hSize,  0.5 * hSize, 0.0),
        vec3(0.5 * hSize,   0.5 * hSize, 0.0),
        vec3(-0.5 * hSize,  1.0 * hSize, 0.0),
        vec3(0.5 * hSize,   1.0 * hSize, 0.0),
        vec3(-0.5 * hSize,  1.5 * hSize, 0.0),
        vec3(0.5 * hSize,   1.5 * hSize, 0.0),
        vec3(-0.5 * hSize,  2.0 * hSize, 0.0),
        vec3(0.5 * hSize,   2.0 * hSize, 0.0)
    };

    
    std::vector <unsigned int> indices = {
        0, 1, 3,
        0, 3, 2,
        2, 3, 5,
        2, 5, 4,
        4, 5, 7,
        4, 7, 6,
        6, 7, 9,
        6, 9, 8
    };
    
    std::vector<vec3> colors;
    colors.reserve(vertices.size());
    
    for (int i = 0; i < vertices.size(); ++i) {
        colors.emplace_back(_color);
    }
    
    std::vector<vec3> normals = std::vector<vec3>(_vertices.size(), vec3(0,1,0));
    
    _vertices = std::move(vertices);
    _normals = std::move(normals);
    _indices = std::move(indices);
    _colors = std::move(colors);
    
}

