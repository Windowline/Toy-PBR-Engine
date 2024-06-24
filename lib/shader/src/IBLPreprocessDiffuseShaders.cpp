#include "IBLPreprocessDiffuseShaders.hpp"

const char* vertexCubemap = R(
        layout (location = 0) in vec3 aPos;

        out vec3 WorldPos;

        uniform mat4 u_projMat;
        uniform mat4 u_viewMat;

        void main()
        {
            WorldPos = aPos;
            gl_Position =  u_projMat * u_viewMat * vec4(WorldPos, 1.0);
        }
);

const char* fragmentE2H = R(
        out vec4 FragColor;
        in vec3 WorldPos;

        uniform sampler2D u_equirectangularMap;

        const vec2 invAtan = vec2(0.1591, 0.3183);

        vec2 SampleSphericalMap(vec3 v) {
            vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
            uv *= invAtan;
            uv += 0.5;
            return uv;
        }

        void main() {
            vec2 uv = SampleSphericalMap(normalize(WorldPos));
            vec3 color = texture(u_equirectangularMap, uv).rgb;

            FragColor = vec4(color, 1.0);
        }
);

const char* fragmentIrradianceConv = R(
        out vec4 FragColor;
        in vec3 WorldPos;

        uniform samplerCube u_environmentMap;

        const float PI = 3.14159265359;

        void main()
        {
            vec3 N = normalize(WorldPos);

            vec3 irradiance = vec3(0.0);

            // tangent space calculation from origin point
            vec3 up    = vec3(0.0, 1.0, 0.0);
            vec3 right = normalize(cross(up, N));
            up         = normalize(cross(N, right));

            float sampleDelta = 0.025;
            float nrSamples = 0.0f;
            for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
            {
                for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
                {
                    // spherical to cartesian (in tangent space)
                    vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
                    // tangent space to world
                    vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

                    irradiance += texture(u_environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
                    nrSamples++;
                }
            }
            irradiance = PI * irradiance * (1.0 / float(nrSamples));

            FragColor = vec4(irradiance, 1.0);
        }
);




// # EquirectangularToCubemapShader #
EquirectangularToCubemapShader::EquirectangularToCubemapShader() {
    this->load();
    basicUniformLoc();
    _equirectangularMapLoc =  glGetUniformLocation(_programID, "u_equirectangularMap");
}

bool EquirectangularToCubemapShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexCubemap);
    string fShader = string("#version 330 core \n") + string(fragmentE2H);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void EquirectangularToCubemapShader::useProgram() {
    glUseProgram(_programID);
    assert(_equirectangularMapLoc != -1);
    glUniform1i(_equirectangularMapLoc, 0);
}




// # IrradianceShader #
IrradianceShader::IrradianceShader() {
    this->load();
    basicUniformLoc();
    _environmentMapLoc =  glGetUniformLocation(_programID, "u_environmentMap");
}

bool IrradianceShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexCubemap);
    string fShader = string("#version 330 core \n") + string(fragmentIrradianceConv);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void IrradianceShader::useProgram() {
    glUseProgram(_programID);
    assert(_environmentMapLoc != -1);
    glUniform1i(_environmentMapLoc, 0);
}
