#include "PBRShader.hpp"
#include <array>
#include "GLUtilGeometry.hpp"
#include <iostream>

const char* vertexPBR = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;

        out vec3 v_worldPos;
        out vec3 v_normal;

        uniform mat4 u_projMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_worldMat;
        uniform mat4 u_worldNormalMat;

        void main() {
            v_worldPos = vec3(u_worldMat * vec4(a_position, 1.0));
            v_normal = normalize((u_worldNormalMat * vec4(a_normal, 0.0)).xyz);
            gl_Position =  u_projMat * u_viewMat * vec4(v_worldPos, 1.0);
        }
);

const char* fragmentPBR = R(
        out vec4 FragColor;
        in vec3 v_worldPos;
        in vec3 v_normal;

        uniform vec3 u_albedo;
        uniform float u_metallic;
        uniform float u_roughness;
        uniform float u_ao;

        uniform samplerCube u_irradianceMap;
        uniform samplerCube u_prefilterMap;
        uniform sampler2D u_brdfLUT;

        uniform vec3 u_lightPositions[4];
        uniform vec3 u_lightColors[4];
        uniform vec3 u_eyePos;

        const float PI = 3.14159265359;

        float DistributionGGX(vec3 N, vec3 H, float roughness) {
            float a = roughness*roughness;
            float a2 = a*a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH*NdotH;

            float nom   = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = PI * denom * denom;

            return nom / denom;
        }

        float GeometrySchlickGGX(float NdotV, float roughness) {
            float r = (roughness + 1.0);
            float k = (r*r) / 8.0;

            float nom   = NdotV;
            float denom = NdotV * (1.0 - k) + k;

            return nom / denom;
        }

        float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
            float NdotV = max(dot(N, V), 0.0);
            float NdotL = max(dot(N, L), 0.0);
            float ggx2 = GeometrySchlickGGX(NdotV, roughness);
            float ggx1 = GeometrySchlickGGX(NdotL, roughness);

            return ggx1 * ggx2;
        }

        vec3 fresnelSchlick(float cosTheta, vec3 F0) {
            return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
        }

        vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
            return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
        }

        void main() {
            vec3 N = v_normal;
            vec3 V = normalize(u_eyePos - v_worldPos);
            vec3 R = reflect(-V, N);

            // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
            // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
            vec3 F0 = vec3(0.04);
            F0 = mix(F0, u_albedo, u_metallic);

            // reflectance equation
            vec3 Lo = vec3(0.0);
            for(int i = 0; i < 4; ++i)
            {
                // calculate per-light radiance
                vec3 L = normalize(u_lightPositions[i] - v_worldPos);
                vec3 H = normalize(V + L);
                float distance = length(u_lightPositions[i] - v_worldPos);
                float attenuation = 1.0 / (distance * distance);
                vec3 radiance = u_lightColors[i] * attenuation;

                // Cook-Torrance BRDF
                float NDF = DistributionGGX(N, H, u_roughness);
                float G   = GeometrySmith(N, V, L, u_roughness);
                vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

                vec3 numerator    = NDF * G * F;
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
                vec3 specular = numerator / denominator;

                // kS is equal to Fresnel
                vec3 kS = F;
                // for energy conservation, the diffuse and specular light can't
                // be above 1.0 (unless the surface emits light); to preserve this
                // relationship the diffuse component (kD) should equal 1.0 - kS.
                vec3 kD = vec3(1.0) - kS;
                // multiply kD by the inverse metalness such that only non-metals
                // have diffuse lighting, or a linear blend if partly metal (pure metals
                // have no diffuse light).
                kD *= 1.0 - u_metallic;

                // scale light by NdotL
                float NdotL = max(dot(N, L), 0.0);

                // add to outgoing radiance Lo
                Lo += (kD * u_albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
            }

            // ambient lighting (we now use IBL as the ambient term)
            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, u_roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - u_metallic;

            vec3 irradiance = texture(u_irradianceMap, N).rgb;
            vec3 diffuse = irradiance * u_albedo;

            // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(u_prefilterMap, R,  u_roughness * MAX_REFLECTION_LOD).rgb;
            vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), u_roughness)).rg;
            vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

            vec3 ambient = (kD * diffuse + specular) * u_ao;

            vec3 color = ambient + Lo;

            // HDR tonemapping
            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2));

            FragColor = vec4(color , 1.0);
        }
);

PBRShader::PBRShader() {
    this->load();
    basicUniformLoc();

    _albedoLoc = glGetUniformLocation(_programID, "u_albedo");
    _metallicLoc = glGetUniformLocation(_programID, "u_metallic");
    _roughnessLoc = glGetUniformLocation(_programID, "u_roughness");
    _aoLoc = glGetUniformLocation(_programID, "u_ao");

    _lightPositionsLoc = glGetUniformLocation(_programID, "u_lightPositions");
    _lightColorsLoc = glGetUniformLocation(_programID, "u_lightColors");
    _camPosLoc = glGetUniformLocation(_programID, "u_eyePos");

    _irradianceMapLoc = glGetUniformLocation(_programID, "u_irradianceMap");
    _prefilterMapLoc = glGetUniformLocation(_programID, "u_prefilterMap");
    _brdfLUTLoc = glGetUniformLocation(_programID, "u_brdfLUT");
}

bool PBRShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexPBR);
    string fShader = string("#version 330 core \n") + string(fragmentPBR);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void PBRShader::useProgram() {
    glUseProgram(_programID);

    constexpr int TEXTURE_COUNT = 3;

    std::array<GLint, TEXTURE_COUNT> textureLocs = {
            _irradianceMapLoc, _prefilterMapLoc, _brdfLUTLoc
    };

    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        if (textureLocs[i] == -1)
            std::cout << "invalid: " << i << std::endl;

        assert(textureLocs[i] != -1);
        glUniform1i(textureLocs[i], i);
        GLUtil::GL_ERROR_LOG();
    }
}

