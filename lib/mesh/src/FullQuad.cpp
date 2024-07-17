#include "FullQuad.hpp"

#include <glad/glad.h>
#include <vector>

FullQuad::FullQuad(string name) {
    _name = name;

    vector<float> vertices = {
            -1.f, -1.f, 0.f, 0.f, // lb
            -1.f, 1.f, 0.f, 1.f, // lt
            1.f, -1.f, 1.f, 0.f, // rb
            1.f, 1.f, 1.f, 1.f, // rt
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //pos 2d
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

void FullQuad::render(int instanceCount) const {
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (instanceCount == 1)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);
    else
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, (GLsizei)4, instanceCount);
}