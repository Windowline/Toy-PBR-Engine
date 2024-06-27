#ifndef TOYRENDERER_MESHBASIC_H
#define TOYRENDERER_MESHBASIC_H

#include <string>
#include <cassert>
#include <glad/glad.h>
#include <vector>

#include "Vector.hpp"

using namespace std;

class MeshBasic {
public:
    virtual void render() const {
        assert(_VAO != 0 && _indSize !=0);
        glBindVertexArray(_VAO);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glDrawElements(GL_TRIANGLES, _indSize, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    };

    std::string name() const {
        return _name;
    }

    virtual ~MeshBasic() {
        if (_VAO != 0)
            glDeleteVertexArrays(1, &_VAO);
        if (_VBO != 0)
            glDeleteBuffers(1, &_VBO);
        if (_EBO != 0)
            glDeleteBuffers(1, &_EBO);
    }

protected:
    void buildVAO(const vector<vec3>& positions, const vector<vec3>& colors, const vector<vec3>& normals, const vector<unsigned int>& indices) {
        _indSize = indices.size();
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

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);
        glGenBuffers(1, &_EBO);

        glBindVertexArray(_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        unsigned int stride = (3 + 3 + 3) * sizeof(float);

        //pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        //color
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        //normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    std::string _name;
    unsigned int _VAO = 0;
    unsigned int _VBO = 0;
    unsigned int _EBO = 0;
    int _indSize = 0;
};

#endif //TOYRENDERER_MESHBASIC_H
