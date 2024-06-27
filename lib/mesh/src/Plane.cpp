#include "Plane.hpp"
#include <glad/glad.h>

using namespace std;


Plane::Plane(float size, vec3 color, string name) : _size(size), _color(std::move(color)) {
    _name = name;

    float hSize = _size / 2.f;

    vector<vec3> positions = {
        vec3(-hSize, -hSize, 0),
        vec3(hSize, -hSize, 0),
        vec3(hSize, hSize, 0),
        vec3(-hSize, hSize, 0),
    };

    vec3 normal = vec3(0, 0, 1);
    vector<vec3> normals = {normal, normal, normal, normal};
    vector<vec3> colors = {_color, _color, _color, _color };

    vector<float> vertices;
    vertices.reserve(positions.size() * 3 * 3);

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

    vector<unsigned int> indices = {
         0,  1,  2,
         0,  2,  3,
    };

    MeshBasic::buildVAO(positions, colors, normals, indices);
}