#include "SSAOShader.hpp"
#include "GLUtilGeometry.hpp"

const char* vertexSSAOTmp = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;
        out vec2 v_texCoord;

        void main() {
            v_texCoord = a_texCoord;
            vec4 pos = vec4(a_position, 0.0, 1.0);
           gl_Position = pos;
        }
);

const char* fragmentSSAOTmp = R(
        layout (location = 0) out vec4 fragColor; //TODO: float
        in vec2 v_texCoord;
        uniform sampler2D u_viewNormalMap;//xyz: view normal, w: ndc depth
        uniform sampler2D u_noiseMap;  // 접선공간상의 임의 회전
        uniform vec3 u_samples[64];
        uniform vec2 u_screenSize;
        uniform mat4 u_projMat;
        uniform mat4 u_invProjMat;

        vec3 reconstructViewPos(float ndcDepth) {
            vec4 ndc = vec4(v_texCoord.x * 2.0 - 1.0,
                            v_texCoord.y * 2.0 - 1.0,
                            ndcDepth,
                            1.0);
            ndc = u_invProjMat * ndc;
            vec3 view = vec3(ndc.xyz / ndc.w);

            return view;
        }

        void main() {
            float radius = 4.0;
            float bias = 0.03;
            vec2 noiseScale = u_screenSize / 4.0;

            vec4 viewNormalMap = texture(u_viewNormalMap, v_texCoord);
            vec3 viewNormal = normalize(viewNormalMap.xyz); // view normal
            float ndcDepth = viewNormalMap.w; // ndc depth
            vec3 viewPos = reconstructViewPos(ndcDepth);

            vec3 randomVec = texture(u_noiseMap, v_texCoord * noiseScale).xyz;

            vec3 viewTangent = normalize(randomVec - viewNormal * dot(randomVec, viewNormal));
            vec3 viewBitangent = cross(viewNormal, viewTangent);
            mat3 viewTBN = mat3(viewTangent, viewBitangent, viewNormal);

            float occlusion = 0.0;

            for(int i = 0; i < 64; ++i) {
                vec3 viewSamplePos = viewTBN * u_samples[i]; //노이즈 샘플을 뷰공간으로 반영
                viewSamplePos = viewPos + viewSamplePos * radius;

                vec4 offset = vec4(viewSamplePos, 1.0);
                offset = u_projMat * offset;
                offset.xyz /= offset.w;
                offset.xyz = offset.xyz * 0.5 + 0.5;

                float sampleDepth = reconstructViewPos(texture(u_viewNormalMap, offset.xy).w).z;

                float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));
                occlusion += (sampleDepth >= viewSamplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
            }

            occlusion = 1.0 - (occlusion / 64.0);
            fragColor = vec4(vec3(occlusion), 1.0);
        }
);



SSAOShader::SSAOShader() {
    this->load();
    basicUniformLoc();
    _viewNormalMapUniformLoc = glGetUniformLocation(_programID, "u_viewNormalMap");
    _noiseMapUniformLoc = glGetUniformLocation(_programID, "u_noiseMap");
    _samplesUniformLoc = glGetUniformLocation(_programID, "u_samples");
    _screenSizeUniformLoc = glGetUniformLocation(_programID, "u_screenSize");

    assert(_viewNormalMapUniformLoc != -1);
    assert(_noiseMapUniformLoc != -1);
    assert(_samplesUniformLoc != -1);
    assert(_screenSizeUniformLoc != -1);
}

bool SSAOShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexSSAOTmp);
    string fShader = string("#version 330 core \n") + string(fragmentSSAOTmp);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void SSAOShader::useProgram() {
    glUseProgram(_programID);

    glUniform1i(_viewNormalMapUniformLoc, 1);
    GLUtil::GL_ERROR_LOG();

    glUniform1i(_noiseMapUniformLoc, 2);
    GLUtil::GL_ERROR_LOG();
}