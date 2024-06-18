#include "Triangle.hpp"
#include <glad/glad.h>

Triangle::Triangle() : _VAO(-1), _VBO(-1), _EBO(-1) {
    float vertices[] = {
            -0.5f, -0.5f, 0.0f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, // left
            0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, // right
            0.0f,  0.5f, 0.0f,  0.f, 0.f, 1.f, 0.f, 0.f, 1.f // top
    };

    unsigned int index[] = {
            0, 1, 2
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Triangle::render() const {
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}