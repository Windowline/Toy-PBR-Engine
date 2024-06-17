#include "../include/SSAOShader.hpp"
#include "GLUtilGeometry.hpp"

static GLbyte vShader[] =
"#version 300 es                                                            \n"
"precision highp float;                                                     \n"

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
"#version 300 es                                                            \n"
"precision highp float;                                                     \n"

"uniform sampler2D u_posTexture;                                            \n"
"uniform sampler2D u_normalTexture;                                         \n"
"uniform sampler2D u_noiseTexture;                                          \n" // 접선공간상의 임의 회전
"uniform vec3 u_samples[64];                                                \n"
"uniform vec2 u_screenSize;                                                 \n"
"uniform mat4 u_viewMat;                                                    \n"
"uniform mat4 u_projMat;                                                    \n"

"in vec2 v_texCoord;                                                        \n"

"layout (location = 0) out vec4 fragColor;                                  \n"                 //todo: float

"void main(void)                                                                            \n"
"{                                                                                          \n"
"    vec2 noiseScale = u_screenSize / 4.0;                                                  \n"
"    vec3 worldPos = texture(u_posTexture, v_texCoord).xyz;                                 \n"
"    vec3 viewPos = (u_viewMat * vec4(worldPos, 1.0)).xyz;                                  \n"
"    vec3 randomVec = texture(u_noiseTexture, v_texCoord * noiseScale).rgb;                 \n"
"    vec3 N = texture(u_normalTexture, v_texCoord).rgb;                                     \n"
"    mat4 normalViewMat = transpose(inverse(u_viewMat));                                    \n "
"    N = (normalViewMat * vec4(N, 0)).xyz;                                                  \n"
"    N = normalize(N);                                                                      \n"
"    vec3 T = normalize(randomVec - N * dot(randomVec, N));                                 \n"
"    vec3 B = cross(N, T);                                                                  \n"
"    mat3 TBN = mat3(T, B, N);                                                              \n"
"    float radius = 8.0;                                                                    \n"
"    float occlusion = 0.0;                                                                 \n"
"    for (int i = 0; i < 64; ++i) {                                                         \n"
"        vec3 samplePos = TBN * u_samples[i];                                               \n"
"        samplePos = viewPos + samplePos * radius;                                         \n"

"        vec4 offset = u_projMat * vec4(samplePos, 1.0);                                    \n"
"        offset = u_projMat * offset;                                                       \n"
"        offset.xyz /= offset.w;                                                            \n"
"        offset.xyz = offset.xyz * 0.5 + 0.5;                                               \n"
"        float sampleDepth = texture(u_posTexture, offset.xy).z;                            \n"
//"        occlusion += (sampleDepth >= samplePos.z + 0.0001 ? 1.0 : 0.0);                  \n"
"        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleDepth));   \n"
"        occlusion += (sampleDepth >= samplePos.z + 0.0001 ? 1.0 : 0.0) * rangeCheck;       \n"
"     }                                                                                     \n"
"     occlusion = 1.0 - (occlusion / 64.0);                                                 \n"
"     fragColor = vec4(vec3(occlusion), 1.0);                                               \n"
"}                                                                                          \n";




SSAOShader::SSAOShader() : ShaderBase() {
    
    _texCoordAttribLocation = Shader_Invalid_Location;
    
    _posTextureUniformLocation = Shader_Invalid_Location;
    _normalTextureUniformLocation = Shader_Invalid_Location;
    _noiseTextureUniformLocation = Shader_Invalid_Location;
    
    _samplesUniformLocation = Shader_Invalid_Location;
    _screenSizeUniformLocation = Shader_Invalid_Location;
    _projMatUniformLocation = Shader_Invalid_Location;
    _viewMatUniformLocation = Shader_Invalid_Location;
}

bool SSAOShader::load() {
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
    noiseTextureUniformLocation();
    samplesUniformLocation();
    screenSizeUniformLocation();
    
    viewMatUniformLocation();
    projMatUniformLocation();
    
    
    return true;
}


void SSAOShader::useProgram() {
    ShaderBase::useProgram();
    
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_texCoordAttribLocation);
    
    const int MAX_TEXTURE = 3;
    
    std::array<GLint, MAX_TEXTURE> uniformLocs {
        _posTextureUniformLocation, _normalTextureUniformLocation, _noiseTextureUniformLocation
    };
    
    for (int i = 0; i < MAX_TEXTURE; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        GLUtil::GL_ERROR_LOG();
        glUniform1i(uniformLocs[i], i);
        GLUtil::GL_ERROR_LOG();
    }
}
