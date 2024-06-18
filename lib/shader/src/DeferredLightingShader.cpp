#include "DeferredLightingShader.hpp"
#include "GLUtilGeometry.hpp"


static GLbyte vShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                   \n"

"in vec2 a_position;                                                        \n"
"in vec2 a_texCoord;                                                        \n"
"out vec2 v_texCoord;                                                       \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"   v_texCoord = a_texCoord;                                                \n"
"   gl_Position = vec4(a_position, 0.0, 1.0);                               \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                   \n"

"uniform sampler2D u_posTexture;                                            \n"
"uniform sampler2D u_normalTexture;                                         \n"
"uniform sampler2D u_albedoTexture;                                         \n"
"uniform sampler2D u_shadowDepth;                                           \n"
"uniform sampler2D u_ssaoTexture;                                           \n"

"uniform vec3 u_worldEyePos;                                                \n"
"uniform vec3 u_worldLightPos[5];                                           \n"
"uniform int u_lightCount;                                                  \n"

"uniform vec3 u_ambientColor;                                               \n"
"uniform vec3 u_diffuseColor;                                               \n"
"uniform vec3 u_specularColor;                                              \n"

"uniform mat4 u_shadowViewProjectionMat;                                    \n"

"in vec2 v_texCoord;                                                        \n"
"layout (location = 0) out vec4 fragColor;                                  \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"    vec3 albedo = texture(u_albedoTexture, v_texCoord).rgb;                \n"
"    vec3 worldPos = texture(u_posTexture, v_texCoord).rgb;                 \n"
"    float ao = texture(u_ssaoTexture,  v_texCoord).r;                      \n"
"    vec3 N = texture(u_normalTexture, v_texCoord).rgb;                     \n"
"    vec3 E = normalize(worldPos - u_worldEyePos);                          \n"
"    vec3 color = albedo * u_ambientColor * ao;                             \n"

"    vec4 shadowClipPos = u_shadowViewProjectionMat * vec4(worldPos, 1.0);  \n"
"    vec2 shadowDepthUV = shadowClipPos.xy / shadowClipPos.w;               \n"
"    shadowDepthUV = shadowDepthUV * 0.5 + 0.5;                             \n"
"    float shadowDepth = texture(u_shadowDepth, shadowDepthUV).x;           \n"
"    float curDepth = shadowClipPos.z / shadowClipPos.w;                    \n"
"    bool shadow = curDepth > shadowDepth + 0.005;                          \n"
"    for (int i = 0; i < u_lightCount; ++i) {                               \n"
"       vec3 L = normalize(worldPos - u_worldLightPos[i]);                  \n"
"       float df = shadow ? 0.0 : max(0.0, dot(N, -L));                     \n"      // 그림자 지는 영역은 난반사광과 경면광을 제외합니다.
"       color += (albedo * df * u_diffuseColor);                            \n"
"       float sf = shadow ? 0.0 : pow(clamp(dot(reflect(-L, N), E), 0.0, 1.0), 24.0);   \n"
"       color += (sf * u_specularColor);                                   \n"
"    }                                                                      \n"
"    color = clamp(color, 0.0, 1.0);                                        \n"
"    fragColor = vec4(color, 1.0);                                          \n"
"}                                                                          \n";



DeferredLightingShader::DeferredLightingShader() : ShaderBase() {
    
    _texCoordAttribLocation = Shader_Invalid_Location;
    
    _posTextureUniformLocation = Shader_Invalid_Location;
    _normalTextureUniformLocation = Shader_Invalid_Location;
    _albedoTextureUniformLocation = Shader_Invalid_Location;
    _shadowDepthUniformLocation = Shader_Invalid_Location;
    
    _worldLightPosUniformLocation = Shader_Invalid_Location;
    _diffuseColorUniformLocation = Shader_Invalid_Location;
    _specularColorUniformLocation = Shader_Invalid_Location;
    _ambientColorUniformLocation = Shader_Invalid_Location;
    _worldEyePositionUniformLocation = Shader_Invalid_Location;
    
    _shadowViewProjectionMatLocation = Shader_Invalid_Location;
    
    _lightCountUniformLocation = Shader_Invalid_Location;
}

bool DeferredLightingShader::load() {
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == INVALID_GL_ID) {
        return false;
    }
    
    //attrib
    positionAttribLocation();
    texCoordAttribLocation();
    
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

    return true;
}


void DeferredLightingShader::useProgram() {
    ShaderBase::useProgram();
    
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_texCoordAttribLocation);
    
    const int MAX_TEXTURE = 5;
    
    std::array<GLint, MAX_TEXTURE> uniformLocs {
        _posTextureUniformLocation, _normalTextureUniformLocation,
        _albedoTextureUniformLocation, _shadowDepthUniformLocation, _ssaoTextureUniformLocation
    };
    
    for (int i = 0; i < MAX_TEXTURE; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        GLUtil::GL_ERROR_LOG();
        glUniform1i(uniformLocs[i], i);
        GLUtil::GL_ERROR_LOG();
    }

}
