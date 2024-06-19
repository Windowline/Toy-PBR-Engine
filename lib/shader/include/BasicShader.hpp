#ifndef TOYRENDERER_TESTSHADER_TMP_H
#define TOYRENDERER_TESTSHADER_TMP_H

#include <cassert>
#include <string>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define R(...) #__VA_ARGS__

using string = std::string;

class BasicShader {
public:
    BasicShader();
    virtual bool load();
    virtual void useProgram();

    bool isLoaded() const {
        return _programID != 0;
    }

    bool unload() {
        if (_programID) {
            glDeleteProgram (_programID);
        }

        _programID = 0;
        return true;
    }

    int getProgramID() const {
        return _programID;
    }

    void worldMatUniformMatrix4fv(const GLfloat *value) {
        assert(_worldMatLoc != -1);
        glUniformMatrix4fv(_worldMatLoc, 1, GL_FALSE, value);
    }

    void viewMatUniformMatrix4fv(const GLfloat *value) {
        assert(_viewMatLoc != -1);
        glUniformMatrix4fv(_viewMatLoc, 1, GL_FALSE, value);
    }

    void projMatUniformMatrix4fv(const GLfloat *value) {
        assert(_projMatLoc != -1);
        glUniformMatrix4fv(_projMatLoc, 1, GL_FALSE, value);
    }

    void worldNormalMatUniformMatrix4fv(const GLfloat *value) {
        assert(_worldNormalMatLoc != -1);
        glUniformMatrix4fv(_worldNormalMatLoc, 1, GL_FALSE, value);
    }


protected:
    void basicUniformLoc() {
        _worldMatLoc = glGetUniformLocation(_programID, "u_worldMat");
        _viewMatLoc = glGetUniformLocation(_programID, "u_viewMat");
        _projMatLoc = glGetUniformLocation(_programID, "u_projMat");
        _worldNormalMatLoc = glGetUniformLocation(_programID, "u_worldNormalMat");
    }

    GLuint loadShader_tmp(GLenum type, const char *shaderSrc);
    GLuint loadProgram_tmp(const char *vertShaderSrc, const char *fragShaderSrc);

    GLuint _programID;

    GLint _worldMatLoc = -1;
    GLint _viewMatLoc = -1;
    GLint _projMatLoc = -1;
    GLint _worldNormalMatLoc = -1;
};

#endif //TOYRENDERER_TESTSHADER_TMP_H
