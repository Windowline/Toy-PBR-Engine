#include "SSAOShader.hpp"
#include "GLUtilGeometry.hpp"

const char* vertexSSAOTmp = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;
        out vec2 v_texCoord;

        void main()
        {
           v_texCoord = a_texCoord;
           gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentSSAOTmp = R(
        layout (location = 0) out vec4 fragColor; //todo: float
        in vec2 v_texCoord;

        uniform sampler2D u_posTexture;
        uniform sampler2D u_normalTexture;
        uniform sampler2D u_noiseTexture;  // 접선공간상의 임의 회전
        uniform vec3 u_samples[64];
        uniform vec2 u_screenSize;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;


        void main()
        {
            vec2 noiseScale = u_screenSize / 4.0;
            vec3 worldPos = texture(u_posTexture, v_texCoord).xyz;
            vec3 viewPos = (u_viewMat * vec4(worldPos, 1.0)).xyz;
            vec3 randomVec = texture(u_noiseTexture, v_texCoord * noiseScale).rgb;
            vec3 N = texture(u_normalTexture, v_texCoord).rgb;
            mat4 normalViewMat = transpose(inverse(u_viewMat));
            N = (normalViewMat * vec4(N, 0)).xyz;
            N = normalize(N);
            vec3 T = normalize(randomVec - N * dot(randomVec, N));
            vec3 B = cross(N, T);
            mat3 TBN = mat3(T, B, N);
            float radius = 8.0;
            float occlusion = 0.0;
            for (int i = 0; i < 64; ++i) {
                vec3 samplePos = TBN * u_samples[i];
                samplePos = viewPos + samplePos * radius;

                vec4 offset = u_projMat * vec4(samplePos, 1.0);
                offset = u_projMat * offset;
                offset.xyz /= offset.w;
                offset.xyz = offset.xyz * 0.5 + 0.5;
                float sampleDepth = texture(u_posTexture, offset.xy).z;
                occlusion += (sampleDepth >= samplePos.z + 0.0001 ? 1.0 : 0.0);
                float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));
                occlusion += (sampleDepth >= samplePos.z + 0.0001 ? 1.0 : 0.0) * rangeCheck;
             }
             occlusion = 1.0 - (occlusion / 64.0);
             fragColor = vec4(vec3(occlusion), 1.0);
        }
);



SSAOShader::SSAOShader() {
    this->load();
    basicUniformLoc();

    _posTextureUniformLoc = glGetUniformLocation(_programID, "u_posTexture");
    _normalTextureUniformLoc = glGetUniformLocation(_programID, "u_normalTexture");
    _noiseTextureUniformLoc = glGetUniformLocation(_programID, "u_noiseTexture");
    _samplesUniformLoc = glGetUniformLocation(_programID, "u_samples");
    _screenSizeUniformLoc = glGetUniformLocation(_programID, "u_screenSize");

    assert(_posTextureUniformLoc != -1);
    assert(_normalTextureUniformLoc != -1);
    assert(_noiseTextureUniformLoc != -1);
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

    glUniform1i(_posTextureUniformLoc, 0);
    GLUtil::GL_ERROR_LOG();

    glUniform1i(_normalTextureUniformLoc, 1);
    GLUtil::GL_ERROR_LOG();

    glUniform1i(_noiseTextureUniformLoc, 2);
    GLUtil::GL_ERROR_LOG();
}