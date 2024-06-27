#include "IBLPreprocessSpecularShaders.hpp"

const char* vertexCubemap2 = R(
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


const char* fregmentPrefilter = R(
        out vec4 FragColor;
        in vec3 WorldPos;

        uniform samplerCube u_environmentMap;
        uniform float u_roughness;

        const float PI = 3.14159265359;

        float DistributionGGX(vec3 N, vec3 H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;

            float nom   = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = PI * denom * denom;

            return nom / denom;
        }

        float RadicalInverse_VdC(uint bits) {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
        }

        vec2 Hammersley(uint i, uint N) {
            return vec2(float(i)/float(N), RadicalInverse_VdC(i));
        }

        vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
            float a = roughness * roughness;
            //Xi.x, Xi.y --> [0, 1]
            float phi = 2.0 * PI * Xi.x;
            float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y)); //inverse cdf
            float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

            // from spherical coordinates to cartesian coordinates - halfway vector
            vec3 H;
            H.x = cos(phi) * sinTheta;
            H.y = sin(phi) * sinTheta;
            H.z = cosTheta;

            // from tangent-space H vector to world-space sample vector
            vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
            vec3 tangent   = normalize(cross(up, N));
            vec3 bitangent = cross(N, tangent);

            vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
            return normalize(sampleVec);
        }

        void main() {
            vec3 N = normalize(WorldPos);

            // make the simplifying assumption that V equals R equals the normal
            vec3 R = N;
            vec3 V = R;

            const uint SAMPLE_COUNT = 1024u;
            vec3 prefilteredColor = vec3(0.0);
            float totalWeight = 0.0;

            for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
                // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
                vec2 Xi = Hammersley(i, SAMPLE_COUNT);
                vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
                vec3 L  = normalize(2.0 * dot(V, H) * H - V);

                float NdotL = max(dot(N, L), 0.0);

                if(NdotL > 0.0) {
                    // sample from the environment's mip level based on roughness/pdf
                    float D   = DistributionGGX(N, H, u_roughness);
                    float NdotH = max(dot(N, H), 0.0);
                    float HdotV = max(dot(H, V), 0.0);
                    float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

                    float resolution = 512.0; // resolution of source cubemap (per face)
                    float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
                    float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

                    float mipLevel = u_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

                    prefilteredColor += textureLod(u_environmentMap, L, mipLevel).rgb * NdotL;
                    totalWeight      += NdotL;
                }
            }

            prefilteredColor = prefilteredColor / totalWeight;

            FragColor = vec4(prefilteredColor, 1.0);
        }
);

const char* vertexBRDF = R(
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoords;

        out vec2 TexCoords;

        void main()
        {
            TexCoords = aTexCoords;
            gl_Position = vec4(aPos, 1.0);
        }
);



const char* fragmentBRDF = R(
        out vec2 FragColor;
        in vec2 TexCoords;
        const float PI = 3.14159265359;

        float RadicalInverse_VdC(uint bits) {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
        }


        vec2 Hammersley(uint i, uint N) {
            return vec2(float(i)/float(N), RadicalInverse_VdC(i));
        }


        vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
            float a = roughness * roughness;
            float phi = 2.0 * PI * Xi.x;
            float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
            float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

            // from spherical coordinates to cartesian coordinates - halfway vector
            vec3 H;
            H.x = cos(phi) * sinTheta;
            H.y = sin(phi) * sinTheta;
            H.z = cosTheta;

            // from tangent-space H vector to world-space sample vector
            vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
            vec3 tangent   = normalize(cross(up, N));
            vec3 bitangent = cross(N, tangent);

            vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
            return normalize(sampleVec);
        }


        float GeometrySchlickGGX(float NdotV, float roughness) {
            // note that we use a different k for IBL
            float a = roughness;
            float k = (a * a) / 2.0;

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

        vec2 IntegrateBRDF(float NdotV, float roughness) {
            vec3 V;
            V.x = sqrt(1.0 - NdotV*NdotV);
            V.y = 0.0;
            V.z = NdotV;

            float A = 0.0;
            float B = 0.0;

            vec3 N = vec3(0.0, 0.0, 1.0);

            const uint SAMPLE_COUNT = 1024u;
            for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
                // generates a sample vector that's biased towards the
                // preferred alignment direction (importance sampling).
                vec2 Xi = Hammersley(i, SAMPLE_COUNT);
                vec3 H = ImportanceSampleGGX(Xi, N, roughness);
                vec3 L = normalize(2.0 * dot(V, H) * H - V);

                float NdotL = max(L.z, 0.0);
                float NdotH = max(H.z, 0.0);
                float VdotH = max(dot(V, H), 0.0);

                if(NdotL > 0.0) {
                    float G = GeometrySmith(N, V, L, roughness);
                    float G_Vis = (G * VdotH) / (NdotH * NdotV);
                    float Fc = pow(1.0 - VdotH, 5.0);

                    A += (1.0 - Fc) * G_Vis;
                    B += Fc * G_Vis;
                }
            }

            A /= float(SAMPLE_COUNT);
            B /= float(SAMPLE_COUNT);
            return vec2(A, B);
    }

        void main() {
            vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
            FragColor = integratedBRDF;
        }
);






// # prefiter
PrefilterShader::PrefilterShader() {
    this->load();
    basicUniformLoc();
    _environmentMapLoc =  glGetUniformLocation(_programID, "u_environmentMap");
    _roughnessLoc =  glGetUniformLocation(_programID, "u_roughness");
}

bool PrefilterShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexCubemap2);
    string fShader = string("#version 330 core \n") + string(fregmentPrefilter);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void PrefilterShader::useProgram() {
    glUseProgram(_programID);

    assert(_environmentMapLoc != -1);
    glUniform1i(_environmentMapLoc, 0);
}



// # brdf #
BRDFShader::BRDFShader() {
    this->load();
    basicUniformLoc();
}

bool BRDFShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexBRDF);
    string fShader = string("#version 330 core \n") + string(fragmentBRDF);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void BRDFShader::useProgram() {
    glUseProgram(_programID);
//    assert(_environmentMapLoc != -1);
//    glUniform1i(_environmentMapLoc, 0);
}