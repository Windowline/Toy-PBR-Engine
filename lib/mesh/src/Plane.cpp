#include "Plane.hpp"
#include <glad/glad.h>

using namespace std;


Plane::Plane(float size, vec3 color) : _size(size), _color(std::move(color)) {
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

    _indSize = indices.size();

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

void Plane::render() const {
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, _indSize, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}