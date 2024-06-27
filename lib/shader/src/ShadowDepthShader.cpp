#include "ShadowDepthShader.hpp"

const char* vertexShadowDepthShaderTmp = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;

        uniform mat4 u_shadowMVP;

        out vec4 v_shadowClipPos;
        out vec3 v_color;

        void main() {
            v_shadowClipPos = u_shadowMVP * vec4(a_position, 1.0);
            v_color = a_color;
            gl_Position = u_shadowMVP * vec4(a_position, 1.0);
        }
);

const char* fragmentSwhadowDepthShaderTmp = R(
        in vec4 v_shadowClipPos;
        in vec3 v_color;
        layout (location = 0) out vec4 shadowDepth;
        void main()
        {
            shadowDepth = vec4(vec3(v_shadowClipPos.z / v_shadowClipPos.w), 1.0);
//            shadowDepth = vec4(vec3(0.0, 0.0, 1.0), 1.0);
        }
);


ShadowDepthShader::ShadowDepthShader() {
    this->load();
    basicUniformLoc();
    this->shadowMVPUniformLocation();
}

bool ShadowDepthShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexShadowDepthShaderTmp);
    string fShader = string("#version 330 core \n") + string(fragmentSwhadowDepthShaderTmp);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);
    return true;
}