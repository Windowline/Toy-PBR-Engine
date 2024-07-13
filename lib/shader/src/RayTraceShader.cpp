#include "RayTraceShader.hpp"

const char* vertexRayTrace = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;

        out vec2 v_uv;
//        out float v_shaderData;

        void main() {
//            v_shaderData = texelFetch(myTextureBuffer, 2).r;
            v_uv = a_texCoord;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentRayTrace = R(
        struct Ray {
            vec3 org;
            vec3 dir;
        };

        struct Material {
            vec3 color;
        };

        struct Hit {
            bool didHit;
            float dst;
            vec3 pos;
            vec3 N;
            Material mat;
        };

        struct Sphere {
            vec3 pos;
            float r;
            Material mat;
        };

        struct Triangle {
            vec3 posA, posB, posC;
            vec3 NA, NB, NC;
        };

        struct MeshInfo {
            uint firstTriangleIndex;
            uint numTriangles;
            vec3 boundsMin;
            vec3 boundsMax;
            Material mat;
        };

        uniform vec2 u_resolution;
        uniform mat4 u_cameraLocalToWorldMat;
        uniform vec3 u_worldCameraPos;

        uniform samplerBuffer u_normalTBO;
        uniform samplerBuffer u_posTBO;
        uniform int u_triangleSize;

        layout (location = 0) out vec4 fragColor;
        in vec2 v_uv;

        float rand(vec2 co) {
            return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
        }

        vec3 randomHemisphereDir(vec3 N) {
            vec2 randomSeed = gl_FragCoord.xy; // Use fragment coordinates for randomness
            float u = rand(randomSeed);
            float v = rand(randomSeed + vec2(1.0, 1.0));

            float theta = 2.0 * 3.14159265359 * u;
            float phi = acos(2.0 * v - 1.0);

            float x = sin(phi) * cos(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(phi);

            vec3 randomVec = vec3(x, y, z);

            // Ensure the random vector is in the same hemisphere as the N
            if (dot(randomVec, N) < 0.0) {
                randomVec = -randomVec;
            }

            return normalize(randomVec);
        }

        Hit raySphere(Ray ray, vec3 sphereCenter, float sphereRadius) {
            Hit hitInfo;
            hitInfo.didHit = false;

            vec3 oc = ray.org - sphereCenter;

            float a = dot(ray.dir, ray.dir);
            float b = 2.0 * dot(oc, ray.dir);
            float c = dot(oc, oc) - sphereRadius * sphereRadius;
            float discriminant = b * b - 4.0 * a * c;

            if (discriminant >= 0.0) {
                float dst = (-b - sqrt(discriminant)) / (2.0 * a);

                if (dst >= 0.0) {
                    hitInfo.didHit = true;
                    hitInfo.dst = dst;
                    hitInfo.pos = ray.org + ray.dir * dst;
                    hitInfo.N = normalize(hitInfo.pos - sphereCenter);
                }
            }

            return hitInfo;
        }

        Hit rayTriangle(Ray ray, Triangle tri) {
            vec3 edgeAB = tri.posB - tri.posA;
            vec3 edgeAC = tri.posC - tri.posA;
            vec3 N = cross(edgeAB, edgeAC);
            vec3 ao = ray.org - tri.posA;
            vec3 dao = cross(ao, ray.dir);

            float determinant = -dot(ray.dir, N);
            float invDet = 1.0 / determinant;

            //Calculate dst to triangle & barycentric coord of intersection point
            float dst = dot(ao, N) * invDet;
            float u = dot(edgeAC, dao) * invDet;
            float v = -dot(edgeAB, dao) * invDet;
            float w = 1.0 - u - v;

            Hit hit;
            hit.didHit = determinant >= 0.0000001 && dst >= 0 && u >= 0 && v >= 0 && w >= 0;
            hit.pos = ray.org + ray.dir * dst;
            hit.N = normalize(tri.NA * w + tri.NB * u + tri.NC * v);
            hit.dst = dst;
            return hit;
        }

        Hit rayMesh(Ray ray) { // 1 mesh
            Hit closestHit;
            closestHit.didHit = false;
            closestHit.dst = 9999999.0;
            closestHit.mat.color = vec3(0.0);

            for (int triIdx = 0; triIdx < u_triangleSize; ++triIdx) {
                int idxA = triIdx * 3 + 0;
                int idxB = triIdx * 3 + 1;
                int idxC = triIdx * 3 + 2;

                Triangle tri;
                tri.posA = texelFetch(u_posTBO, idxA).xyz;
                tri.posB = texelFetch(u_posTBO, idxB).xyz;
                tri.posC = texelFetch(u_posTBO, idxC).xyz;
                tri.NA = texelFetch(u_normalTBO, idxA).xyz;
                tri.NB = texelFetch(u_normalTBO, idxB).xyz;
                tri.NC = texelFetch(u_normalTBO, idxC).xyz;

                Hit hit = rayTriangle(ray, tri);

                if (hit.didHit && hit.dst < closestHit.dst) {
                    closestHit = hit;
                    closestHit.mat.color = vec3(1.0, 1.0, 1.0);
                }
            }

            return closestHit;
        }



//        Hit rayMesh(Ray ray) {
//            for (int meshIdx = 0; meshIdx < numMeshes; ++meshIdx) {
//                MeshInfo meshInfo = AllMeshInfo[meshIdx];
//
//                if (!rayBoundingBox(ray, meshInfo.boundsMin, meshInfo.boundsMax)) {
//                    continue;
//                }
//
//                for (int i = 0; i < meshInfo.numTriangles; ++i) {
//                    int triIdx = meshInfo.firstTriangleIndex + i;
//                    Triangle tri = Triangles[triIdx];
//                    Hit hit = rayTriangle(ray, tri);
//
//                    if (hit.didHit && hit.dst < closestHit.dst) {
//                        closestHit = hit;
//                        closestHit.meshInfo.mat;
//                    }
//
//                }
//            }
//        }

        Hit rayCollisionSphere(Ray ray) {
            int numSphere = 2;
            Sphere spheres[2];

            spheres[0].pos = vec3(0.0, 0.0, -40.0);
            spheres[0].r = 25.0;
            spheres[0].mat.color = vec3(1.0, 1.0, 1.0);

            spheres[1].pos = vec3(0.0, 0.0, 0.0);
            spheres[1].r = 5.0;
            spheres[1].mat.color = vec3(1.0, 0.0, 0.0);

            Hit closestHit;
            closestHit.didHit = false;
            closestHit.dst = 9999999.0;
            closestHit.mat.color = vec3(0.0);

            for (int i = 0; i < numSphere; ++i) {
                Sphere sphere = spheres[i];
                Hit hit = raySphere(ray, sphere.pos, sphere.r);

                if (hit.didHit && hit.dst < closestHit.dst) {
                    closestHit = hit;
                    closestHit.mat = sphere.mat;
                }
            }

            return closestHit;
        }

        vec3 trace(Ray ray) {
            int MAX_BOUNCE = 5;

            vec3 incomingL = vec3(0.0);
            vec3 rayColor = vec3(1.0);

            for (int i = 0; i < MAX_BOUNCE; ++i) {
                Hit hit = rayCollisionSphere(ray);

                if (hit.didHit) {
                    float s = dot(hit.N, -ray.dir);
                    ray.org = hit.pos;
                    ray.dir = randomHemisphereDir(hit.N);
//                    vec3 emittedL = mat.emissionColor * mat.emissionStrength;
                    vec3 emittedL = vec3(1.0);

                    rayColor *= hit.mat.color * s;
                    incomingL += emittedL * rayColor;
                } else {
//                    incomingL += getEnvColor(ray) * rayColor; // TODO
                    break;
                }
            }

            return incomingL;
        }

        void main() {
            mat4 tmp2 = u_cameraLocalToWorldMat;
            vec4 tmp3 = vec4(texelFetch(u_posTBO, 0).xyz, 1.0);
            vec4 tmp4 = vec4(texelFetch(u_normalTBO, 0).xyz, 1.0);

            vec2 uv = v_uv * 2.0 - 1.0;
            float aspect = u_resolution.x / u_resolution.y;
            uv.x *= aspect;

            Ray ray;
            ray.org = u_worldCameraPos;
            ray.dir = normalize(vec3(uv, -1.0));

            //1 sphere 1 bounce
//            Hit closestHit = rayCollisionSphere(ray);
//            fragColor = vec4(closestHit.mat.color * dot(closestHit.N, -ray.dir), 1.0);

            //2 sphere multi bounce
//            float RAY_SAMPLE_CNT = 4.0;
//            vec3 total = vec3(0.0);
//            for (float i = 0; i < RAY_SAMPLE_CNT; i += 1.0) {
//                total += trace(ray);
//            }
//            fragColor = vec4(total / RAY_SAMPLE_CNT, 1.0);

              Hit hit = rayMesh(ray);
              fragColor = vec4(hit.mat.color, 1.0);
        }
);

RayTraceShader::RayTraceShader() {
    this->load();
    basicUniformLoc();

    _posTBOLoc = glGetUniformLocation(_programID, "u_posTBO");
    _normalTBOLoc = glGetUniformLocation(_programID, "u_normalTBO");

    _cameraPosUniformLoc = glGetUniformLocation(_programID, "u_worldCameraPos");
    _cameraLocalToWorldMatUniformLoc = glGetUniformLocation(_programID, "u_cameraLocalToWorldMat");
    _resolutionUnifromLoc = glGetUniformLocation(_programID, "u_resolution");
    _triangleSizeLoc = glGetUniformLocation(_programID, "u_triangleSize");
}

bool RayTraceShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexRayTrace);
    string fShader = string("#version 330 core \n") + string(fragmentRayTrace);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);
    return true;
}

void RayTraceShader::useProgram() {
    glUseProgram(_programID);

    assert(_posTBOLoc != -1);
    glUniform1i(_posTBOLoc, 0);

    assert(_normalTBOLoc != -1);
    glUniform1i(_normalTBOLoc, 1);
}