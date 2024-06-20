#include "GBufferShader.hpp"

const char* vertexGBufferShaderTmp = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;

        uniform mat4 u_worldMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;
        uniform mat4 u_worldNormalMat;

        out vec3 v_color;
        out vec3 v_position;
        out vec3 v_normal;

        void main()
        {
           vec4 worldPos = u_worldMat * vec4(a_position, 1.0);
           v_normal = normalize((u_worldNormalMat * vec4(a_normal, 0.0)).xyz);

           v_color = a_color;
           v_position = worldPos.xyz;
           gl_Position = u_projMat * u_viewMat * u_worldMat * vec4(a_position, 1.0);
        }
);

const char* fragmentGBufferShaderTmp = R(
        in vec3 v_position;
        in vec3 v_color;
        in vec3 v_normal;

        layout (location = 0) out vec4 gPosition;
        layout (location = 1) out vec4 gNormal;
        layout (location = 2) out vec4 gAlbedo;

        void main()
        {
            gPosition = vec4(v_position, 1.0);
            gNormal = vec4(normalize(v_normal), 1.0);
            gAlbedo = vec4(v_color, 1.0);
        }
);

GBufferShader::GBufferShader() {
    this->load();
    BasicShader::basicUniformLoc();
}

bool GBufferShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexGBufferShaderTmp);
    string fShader = string("#version 330 core \n") + string(fragmentGBufferShaderTmp);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}