#include "DeferredLightingShader.hpp"
#include "GLUtilGeometry.hpp"

const char* vertexDeferred = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;

        out vec2 v_texCoord;

        void main()
        {
           v_texCoord = a_texCoord;
           gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentDeferred = R(
        uniform sampler2D u_posTexture;
        uniform sampler2D u_normalTexture;
        uniform sampler2D u_albedoTexture;
        uniform sampler2D u_shadowDepth;
        uniform sampler2D u_ssaoTexture;

        uniform vec3 u_worldEyePos;
        uniform vec3 u_worldLightPos[5];
        uniform int u_lightCount;

        uniform vec3 u_ambientColor;
        uniform vec3 u_diffuseColor;
        uniform vec3 u_specularColor;

        uniform mat4 u_shadowViewProjectionMat;

        in vec2 v_texCoord;
        layout (location = 0) out vec4 fragColor;

        void main()
        {
            vec3 albedo = texture(u_albedoTexture, v_texCoord).rgb;
            vec3 worldPos = texture(u_posTexture, v_texCoord).rgb;
            float ao = texture(u_ssaoTexture,  v_texCoord).r;
            vec3 N = texture(u_normalTexture, v_texCoord).rgb;
            vec3 E = normalize(worldPos - u_worldEyePos);
            vec3 color = albedo * u_ambientColor * ao;

            vec4 shadowClipPos = u_shadowViewProjectionMat * vec4(worldPos, 1.0);
            vec2 shadowDepthUV = shadowClipPos.xy / shadowClipPos.w;
            shadowDepthUV = shadowDepthUV * 0.5 + 0.5;
            float shadowDepth = texture(u_shadowDepth, shadowDepthUV).x;
            float curDepth = shadowClipPos.z / shadowClipPos.w;
            bool shadow = curDepth > shadowDepth + 0.005;

            for (int i = 0; i < u_lightCount; ++i) {
               vec3 L = normalize(worldPos - u_worldLightPos[i]);
               float df = shadow ? 0.0 : max(0.0, dot(N, -L)); // 그림자 지는 영역은 난반사광과 경면광을 제외합니다.
               color += (albedo * df * u_diffuseColor);
               float sf = shadow ? 0.0 : pow(clamp(dot(reflect(-L, N), E), 0.0, 1.0), 24.0);
               color += (sf * u_specularColor);
            }
            color = clamp(color, 0.0, 1.0);
            fragColor = vec4(color, 1.0);
        }
);

DeferredLightingShader::DeferredLightingShader() {
    this->load();
    basicUniformLoc();

    //uniform
    posTextureUniformLocation();
    normalTextureUniformLocation();
    albedoTextureUniformLocation();

    worldLightPosUniformLocation();
    diffuseColorUniformLocation();
    specularColorUniformLocation();
    ambientColorUniformLocation();
    worldEyePositionUniformLocation();

    shadowViewProjectionMatUniformLocation();

    lightCountUniformLocation();

    shadowDepthUniformLocation();

    ssaoTextureUniformLocation();
}


bool DeferredLightingShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexDeferred);
    string fShader = string("#version 330 core \n") + string(fragmentDeferred);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void DeferredLightingShader::useProgram() {
    glUseProgram(_programID);

    constexpr int TEXTURE_COUNT = 5;

    std::array<GLint, TEXTURE_COUNT> uniformLocs {
            _posTextureUniformLocation, _normalTextureUniformLocation,
            _albedoTextureUniformLocation, _shadowDepthUniformLocation, _ssaoTextureUniformLocation
    };

    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        assert(uniformLocs[i] != -1);
        glUniform1i(uniformLocs[i], i);
        GLUtil::GL_ERROR_LOG();
    }
}