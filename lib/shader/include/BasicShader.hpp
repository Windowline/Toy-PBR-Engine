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

    void worldMatUniformMatrix4fv(const GLfloat *value, int count) {
        assert(_worldMatLoc != -1);
        glUniformMatrix4fv(_worldMatLoc, count, GL_FALSE, value);
    }

    void viewMatUniformMatrix4fv(const GLfloat *value) {
        assert(_viewMatLoc != -1);
        glUniformMatrix4fv(_viewMatLoc, 1, GL_FALSE, value);
    }

    void invViewMatUniformMatrix4fv(const GLfloat *value) {
        assert(_invViewMatLoc != -1);
        glUniformMatrix4fv(_invViewMatLoc, 1, GL_FALSE, value);
    }

    void projMatUniformMatrix4fv(const GLfloat *value) {
        assert(_projMatLoc != -1);
        glUniformMatrix4fv(_projMatLoc, 1, GL_FALSE, value);
    }

    void invProjMatUniformMatrix4fv(const GLfloat *value) {
        assert(_invProjMatLoc != -1);
        glUniformMatrix4fv(_invProjMatLoc, 1, GL_FALSE, value);
    }

    void worldNormalMatUniformMatrix4fv(const GLfloat *value, int count) {
        assert(_worldNormalMatLoc != -1);
        glUniformMatrix4fv(_worldNormalMatLoc, count, GL_FALSE, value);
    }


protected:
    void basicUniformLoc() {
        _worldMatLoc = glGetUniformLocation(_programID, "u_worldMat");
        _viewMatLoc = glGetUniformLocation(_programID, "u_viewMat");
        _invViewMatLoc = glGetUniformLocation(_programID, "u_invViewMat");
        _projMatLoc = glGetUniformLocation(_programID, "u_projMat");
        _invProjMatLoc = glGetUniformLocation(_programID, "u_invProjMat");
        _worldNormalMatLoc = glGetUniformLocation(_programID, "u_worldNormalMat");
    }

    GLuint loadShader(GLenum type, const char *shaderSrc);
    GLuint loadProgram(const char *vertShaderSrc, const char *fragShaderSrc);

    GLuint _programID;

    GLint _worldMatLoc = -1;
    GLint _viewMatLoc = -1;
    GLint _invViewMatLoc = -1;
    GLint _projMatLoc = -1;
    GLint _invProjMatLoc = -1;
    GLint _worldNormalMatLoc = -1;
};

#endif //TOYRENDERER_TESTSHADER_TMP_H
