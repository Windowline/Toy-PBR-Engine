#include "DeferredPBRShader.hpp"
#include "GLUtilGeometry.hpp"

const char* vertexDeferredPBR = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;

        out vec2 v_texCoord;

        void main()
        {
            v_texCoord = a_texCoord;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentDeferredPBR = R(
        in vec2 v_texCoord;
        layout (location = 0) out vec4 fragColor;

        uniform sampler2D u_posTexture;
        uniform sampler2D u_normalTexture;
        uniform sampler2D u_albedoTexture;
        uniform sampler2D u_shadowDepth;
        uniform sampler2D u_ssaoTexture;

        uniform vec3 u_worldEyePos; // = camPos
        uniform vec3 u_worldLightPos[5];
        uniform int u_lightCount;

        uniform vec3 u_ambientColor;
        uniform vec3 u_diffuseColor;
        uniform vec3 u_specularColor;

        uniform mat4 u_shadowViewProjectionMat;

        // ------------------ PBR
//        uniform vec3 albedo; // = u_ambientColor
        uniform float u_metallic;
        uniform float u_roughness;
//        uniform float ao; // = ssao

        uniform samplerCube u_irradianceMap;
        uniform samplerCube u_prefilterMap;
        uniform sampler2D u_brdfLUT;

//        uniform vec3 lightPositions[4]; // = u_worldLightPos
        uniform vec3 u_lightColors[5];
        // ----------------------

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

        vec3 applyIBL(vec3 N, vec3 worldPos, vec3 albedo, float ao) {
            vec3 V = normalize(u_worldEyePos - worldPos);
            vec3 R = reflect(-V, N);

            vec3 F0 = vec3(0.04);
            F0 = mix(F0, albedo, u_metallic);

            // reflectance equation
            vec3 Lo = vec3(0.0);

            for(int i = 0; i < u_lightCount; ++i) {
                // calculate per-light radiance
                vec3 L = normalize(u_worldLightPos[i] - worldPos);
                vec3 H = normalize(V + L);
                float distance = length(u_worldLightPos[i] - worldPos);
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
                Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
            }

            vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, u_roughness);

            vec3 kS = F;
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - u_metallic;

            vec3 irradiance = texture(u_irradianceMap, N).rgb;
            vec3 diffuse = irradiance * albedo;

            // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
            const float MAX_REFLECTION_LOD = 4.0;
            vec3 prefilteredColor = textureLod(u_prefilterMap, R,  u_roughness * MAX_REFLECTION_LOD).rgb;
            vec2 brdf  = texture(u_brdfLUT, vec2(max(dot(N, V), 0.0), u_roughness)).rg;
            vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

            vec3 ambient = (kD * diffuse + specular) * ao;

            vec3 color = ambient + Lo;

            color = color / (color + vec3(1.0));
            // gamma correct
            color = pow(color, vec3(1.0/2.2));

            return color;
        }

        void main() {
            vec3 albedo = texture(u_albedoTexture, v_texCoord).rgb;
            vec3 worldPos = texture(u_posTexture, v_texCoord).rgb;
            float ao = texture(u_ssaoTexture,  v_texCoord).r;
            vec3 N = texture(u_normalTexture, v_texCoord).rgb;
            vec3 E = normalize(worldPos - u_worldEyePos);
            vec3 color = albedo * u_ambientColor * ao;

            if (texture(u_albedoTexture, v_texCoord).a > 0.0) {
                vec3 colorIBL = applyIBL(N, worldPos, albedo, 1.0);
                fragColor = vec4(colorIBL, 1.0);
            } else { //skybox
                fragColor = vec4(albedo, 1.0);
            }

            //tmp
            float rr = u_roughness;
            float mm = u_metallic;
            vec3 cc = u_lightColors[0];
            vec3 prefilteredColor = textureLod(u_prefilterMap, vec3(2.0),  u_roughness).rgb;
            vec2 brdf  = texture(u_brdfLUT, vec2(0.1, 0.1)).rg;
            vec3 irradiance = texture(u_irradianceMap, N).rgb;
            float sssss = texture(u_shadowDepth, vec2(0.1, 0.1)).x;
            vec3 ddd = u_diffuseColor;
            vec3 ssp = u_specularColor;



            vec4 shadowClipPos = u_shadowViewProjectionMat * vec4(worldPos, 1.0);
            vec2 shadowDepthUV = shadowClipPos.xy / shadowClipPos.w;
            shadowDepthUV = shadowDepthUV * 0.5 + 0.5;
            float shadowDepth = texture(u_shadowDepth, shadowDepthUV).x;
            float curDepth = shadowClipPos.z / shadowClipPos.w;
            bool shadow = curDepth > shadowDepth + 0.005;

//            float abeldoAlpha = texture(u_albedoTexture, v_texCoord).a;
//
////            fragColor = vec4(vec3(abeldoAlpha), 1.0);
//
//            if (abeldoAlpha > 0.0) {
//                for (int i = 0; i < u_lightCount; ++i) {
//                    vec3 L = normalize(worldPos - u_worldLightPos[i]);
//                    float df = shadow ? 0.0 : max(0.0, dot(N, -L)); // 그림자 지는 영역은 난반사광과 경면광을 제외합니다.
//                    color += (albedo * df * u_diffuseColor);
//                    float sf = shadow ? 0.0 : pow(clamp(dot(reflect(-L, N), E), 0.0, 1.0), 24.0);
//                    color += (sf * u_specularColor);
//                }
//                color = clamp(color, 0.0, 1.0);
//                fragColor = vec4(color, 1.0);
//            } else {
//                fragColor = vec4(albedo, 1.0);
//            }
        }
);

DeferredPBRShader::DeferredPBRShader() {
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


    //pbr
//    _albedoLoc = glGetUniformLocation(_programID, "albedo");
    _metallicLoc = glGetUniformLocation(_programID, "u_metallic");
    _roughnessLoc = glGetUniformLocation(_programID, "u_roughness");
//    _aoLoc = glGetUniformLocation(_programID, "ao");

    _lightColorsLoc = glGetUniformLocation(_programID, "u_lightColors");
//    _camPosLoc = glGetUniformLocation(_programID, "camPos");

    _irradianceMapLoc = glGetUniformLocation(_programID, "u_irradianceMap");
    _prefilterMapLoc = glGetUniformLocation(_programID, "u_prefilterMap");
    _brdfLUTLoc = glGetUniformLocation(_programID, "u_brdfLUT");
}


bool DeferredPBRShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexDeferredPBR);
    string fShader = string("#version 330 core \n") + string(fragmentDeferredPBR);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void DeferredPBRShader::useProgram() {
    glUseProgram(_programID);

    constexpr int TEXTURE_COUNT = 5 + 3;

    // Deferred
    std::array<GLint, TEXTURE_COUNT> uniformLocs {
            // Deferred
            _posTextureUniformLocation,
            _normalTextureUniformLocation,
            _albedoTextureUniformLocation,
            _shadowDepthUniformLocation,
            _ssaoTextureUniformLocation,
            //PBR
            _irradianceMapLoc,
            _prefilterMapLoc,
            _brdfLUTLoc
    };

    for (int i = 0; i < TEXTURE_COUNT; ++i) {
        if (uniformLocs[i] == - 1) {
            int asdas = 10;
        }
        assert(uniformLocs[i] != -1);
        glUniform1i(uniformLocs[i], i);
        GLUtil::GL_ERROR_LOG();
    }
}