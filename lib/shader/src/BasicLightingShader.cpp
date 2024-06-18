
#include "BasicLightingShader.hpp"

static GLbyte vShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                   \n"

"uniform mat4 u_worldMat;                                                   \n"
"uniform mat4 u_worldViewMat;                                               \n"
"uniform mat4 u_projMat;                                                    \n"
"uniform mat4 u_worldNormalMat;                                              \n"

"uniform vec3 u_worldEyePos;                                                \n"
"uniform vec3 u_worldLightPos[3];                                           \n"

"in vec3 a_position;                                                 \n"
"in vec3 a_normal;                                                   \n"
"in vec3 a_color;                                                    \n"

"out vec3 v_color;                                                      \n"
"out vec3 v_normal;                                                     \n"
"out vec3 v_eyeDir;                                                     \n"
"out vec3 v_lightDir[3];                                                \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"   vec4 worldPos = u_worldMat * vec4(a_position, 1.0);                     \n"
"   v_normal = normalize((u_worldNormalMat * vec4(a_normal, 0.0)).xyz);     \n"
"   v_eyeDir = normalize(worldPos.xyz - u_worldEyePos);                     \n"
"   for (int i = 0; i < 3; ++i) {                                           \n"
"       v_lightDir[i] = normalize(worldPos.xyz - u_worldLightPos[i]);       \n"
"   }                                                                       \n"
"   v_color = a_color;                                                      \n"
"   gl_Position = u_projMat * u_worldViewMat * vec4(a_position, 1.0);       \n"
"}                                                                          \n";

static GLbyte fShader[] =
"#version 330 core                                                            \n"
"precision highp float;                                                   \n"

"uniform vec3 u_ambientColor;                                               \n"
"uniform vec3 u_diffuseColor;                                               \n"
"uniform vec3 u_specularColor;                                              \n"

"in vec3 v_color;                                                           \n"
"in vec3 v_normal;                                                          \n"
"in vec3 v_eyeDir;                                                          \n"
"in vec3 v_lightDir[3];                                                     \n"
"layout (location = 0) out vec4 fragColor;                                  \n"
"                                                                           \n"
"void main(void)                                                            \n"
"{                                                                          \n"
"    vec3 albedo = v_color;                                                 \n"
"    vec3 color = albedo * u_ambientColor;                                  \n"
"    vec3 N = normalize(v_normal);                                          \n"
"    vec3 E = normalize(v_eyeDir);                                          \n"
"    for (int i = 0; i < 3; ++i) {                                          \n"
"        vec3 L = normalize(v_lightDir[i]);                                 \n"
"        float df = max(0.0, dot(N, -L));                                   \n"
"        float sf = pow(clamp(dot(reflect(L, N), E), 0.0, 1.0), 48.0);      \n"
"        color += (albedo * df * u_diffuseColor);                           \n"
"        color += (sf * u_specularColor);                                   \n"
"    }                                                                      \n"
"    color = clamp(color, 0.0, 1.0);                                        \n"
"    fragColor = vec4(color, 1.0);                                          \n"
"}                                                                          \n";



BasicLightingShader::BasicLightingShader() : ShaderBase()
{
    
    _colorAttribLocation = Shader_Invalid_Location;
    _normalAttribLocation = Shader_Invalid_Location;
    
    //uniform
    _worldViewMatUniformLocation = Shader_Invalid_Location;
    _projMatUniformLocation = Shader_Invalid_Location;
    
    _worldLightPosUniformLocation = Shader_Invalid_Location;
    _diffuseColorUniformLocation = Shader_Invalid_Location;
    _specularColorUniformLocation = Shader_Invalid_Location;
    _ambientColorUniformLocation = Shader_Invalid_Location;
    _worldEyePositionUniformLocation = Shader_Invalid_Location;
}
    



bool BasicLightingShader::load()
{
    _programID = loadProgram(reinterpret_cast<const char *>(vShader),
                             reinterpret_cast<const char *>(fShader));
    
    if (_programID == 0) {
        return false;
    }
    
    
    //attrib
    positionAttribLocation();
    colorAttribLocation();
    normalAttribLocation();
    
    
    //uniform
    worldUniformLocation();
    worldViewUniformLocation();
    projUniformLocation();
    worldNormalMatUniformLocation();
    
    worldLightPosUniformLocation();
    diffuseColorUniformLocation();
    specularColorUniformLocation();
    ambientColorUniformLocation();
    worldEyePositionUniformLocation();
    
    return true;
}

void BasicLightingShader::useProgram() {
    
    ShaderBase::useProgram();
    glEnableVertexAttribArray(_positionAttribLocation);
    glEnableVertexAttribArray(_colorAttribLocation);
    glEnableVertexAttribArray(_normalAttribLocation);
    
}
