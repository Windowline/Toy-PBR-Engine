#include "ShadowDepthShaderTmp.hpp"

static const char* vShadowDepthShader =
        "#version 330 core                                                          \n"
        "layout (location = 0) in vec3 a_position;                                  \n"
        "layout (location = 1) in vec3 a_color;                                     \n"

        "out vec4 v_shadowClipPos;                                                  \n"
        "out vec3 v_color;                                                          \n"
        "uniform mat4 u_shadowMVP;                                                  \n"

        "void main()                                                                \n"
        "{                                                                          \n"
        "   v_shadowClipPos = u_shadowMVP * vec4(a_position, 1.0);                  \n"
        "   v_color = a_color;                                                      \n"
        "   gl_Position = u_shadowMVP * vec4(a_position, 1.0);                      \n"
        "}                                                                          \n";

static const char* fShadowDepthShader =
        "#version 330 core                                                                \n"
        "in vec4 v_shadowClipPos;                                                           \n"
        "in vec3 v_color;                                                               \n"
        "layout (location = 0) out vec4 shadowDepth;                                                          \n"
        "void main()                                                                    \n"
        "{                                                                                  \n"
//        "    shadowDepth = vec4(vec3((1.0 - v_shadowClipPos.z) / v_shadowClipPos.w), 1.0);          \n"
        "    shadowDepth = vec4(1.0, 0.0, 0.0, 1.0);          \n"

        "}                                                                                  \n";



ShadowDepthShaderTmp::ShadowDepthShaderTmp() {
    this->load();
    this->shadowMVPUniformLocation();
}

bool ShadowDepthShaderTmp::load() {
    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShadowDepthShader),
                                 reinterpret_cast<const char *>(fShadowDepthShader));

    assert(_programID != 0);
    return true;
}