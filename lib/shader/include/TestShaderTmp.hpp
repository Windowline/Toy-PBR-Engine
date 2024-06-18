#ifndef TOYRENDERER_TESTSHADER_TMP_H
#define TOYRENDERER_TESTSHADER_TMP_H

#include <cassert>
#include <string>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define R(...) #__VA_ARGS__


class TestShaderTmp {
public:
    TestShaderTmp();
    virtual bool load();
    void useProgram();

    void mvpUniformLocation() {
        _mvpUniformLocation =  glGetUniformLocation(_programID, "u_mvp");
    }

    void mvpUniformMatrix4fv(const GLfloat *value) {
        glUniformMatrix4fv(_mvpUniformLocation, 1, GL_FALSE, value);
    }

protected:
    GLuint loadShader_tmp(GLenum type, const char *shaderSrc);
    GLuint loadProgram_tmp(const char *vertShaderSrc, const char *fragShaderSrc);

    GLuint _programID;
    GLint _mvpUniformLocation;
};

#endif //TOYRENDERER_TESTSHADER_TMP_H
