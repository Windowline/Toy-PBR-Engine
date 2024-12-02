#include "GBufferShader.hpp"

const char* vertexGBufferShaderTmp = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;

        uniform mat4 u_worldMat[9];
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;
        uniform mat4 u_worldNormalMat[9];
        uniform float u_isRenderSkyBox;
        uniform vec3 u_color[9];

        out vec3 v_color;
        out vec3 v_worldPos;
        out vec3 v_viewPos;
        out vec3 v_ndcPos;
        out vec3 v_normal;
        out vec3 v_viewNormal;

        void main() {
           mat3 viewNormalMat = transpose(inverse(mat3(u_viewMat * u_worldMat[gl_InstanceID])));

           vec4 worldPos = u_worldMat[gl_InstanceID] * vec4(a_position, 1.0);
           vec4 viewPos = u_viewMat * worldPos;
           vec4 clipPos =  u_projMat * viewPos;

           v_worldPos = worldPos.xyz;
           v_ndcPos = clipPos.xyz / clipPos.w;

           v_viewPos = viewPos.xyz;
           v_normal = (u_worldNormalMat[gl_InstanceID] * vec4(a_normal, 0.0)).xyz;
           v_viewNormal = viewNormalMat * a_normal;

           v_color = u_color[gl_InstanceID]; //v_color = a_color;

           if (u_isRenderSkyBox > 0.5) {
               vec4 clipPosForSkyBox = u_projMat * mat4(mat3(u_viewMat)) * worldPos;
               gl_Position = clipPosForSkyBox.xyww;
           } else {
//               vec4 clipPos = u_projMat * u_viewMat * worldPos; // TODO: FIX
               gl_Position = clipPos;
           }

        }
);

const char* fragmentGBufferShaderTmp = R(
        in vec3 v_worldPos;
        in vec3 v_viewPos;
        in vec3 v_ndcPos;
        in vec3 v_normal;
        in vec3 v_viewNormal;
        in vec3 v_color;

        layout (location = 0) out vec4 gPosition; // TODO: remove
        layout (location = 1) out vec4 gNormal; // w: depth
        layout (location = 2) out vec4 gAlbedo;
        layout (location = 3) out vec4 gViewPosition; // TODO: remove
        layout (location = 4) out vec4 gViewNormal; // w: view depth

        uniform samplerCube u_environmentMap;
        uniform float u_isRenderSkyBox;

        void main() {
            gPosition = vec4(v_worldPos, 1.0);
            gViewPosition = vec4(v_viewPos, 1.0);

//            gNormal = vec4(normalize(v_normal), 1.0);
//            gViewNormal = vec4(normalize(v_viewNormal), 1.0);
            gNormal.xyz = normalize(v_normal);
            gNormal.w = v_ndcPos.z;

            gViewNormal.xyz = normalize(v_viewNormal);
            gViewNormal.w = v_viewPos.z;

            if (u_isRenderSkyBox > 0.5) {
                vec3 envColor = textureLod(u_environmentMap, v_worldPos, 0.0).rgb;
                // HDR tonemap and gamma correct
                envColor = envColor / (envColor + vec3(1.0));
                envColor = pow(envColor, vec3(1.0 / 2.2));
                gAlbedo = vec4(envColor, -1.0);
            } else {
                gAlbedo = vec4(v_color, 1.0);
            }
        }
);

GBufferShader::GBufferShader() {
    this->load();
    basicUniformLoc();

    _isRenderSkyBoxLoc = glGetUniformLocation(_programID, "u_isRenderSkyBox");
    _colorLoc = glGetUniformLocation(_programID, "u_color");
    _envCubeMapLoc =  glGetUniformLocation(_programID, "u_environmentMap");
}

bool GBufferShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexGBufferShaderTmp);
    string fShader = string("#version 330 core \n") + string(fragmentGBufferShaderTmp);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void GBufferShader::useProgram() {
    glUseProgram(_programID);
    assert(_envCubeMapLoc != -1);
    glUniform1i(_envCubeMapLoc, 0);
}