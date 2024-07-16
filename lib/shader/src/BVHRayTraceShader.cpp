#include "BVHRayTraceShader.hpp"

const char* vertexBVHRayTrace = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;
        out vec2 v_uv;

        void main() {
            v_uv = a_texCoord;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentBVHRayTrace = R(
        struct Ray {
            vec3 org;
            vec3 dir;
            vec3 invDir;
        };

        struct Material {
            vec3 color;
            int type; // 0:Diffuse 1:Metal
        };

        struct Mesh {
            int nodeOffset;
            int triangleOffset;
            mat4 worldToLocalMatrix;
            Material mat;
        };

        struct Hit {
            bool didHit;
            float dst;
            vec3 pos;
            vec3 N;
            Material mat;
        };

        struct Triangle {
            vec3 posA, posB, posC;
            vec3 NA, NB, NC;
        };

        struct Sphere {
            vec3 pos;
            float r;
            Material mat;
        };

        struct AABB {
            vec3 minBounds;
            vec3 maxBounds;
            Material mat;
        };

        struct BVHNode {
            int triangleIdx;
            int triangleCnt;
            int nodeIdx;
            vec3 minBounds;
            vec3 maxBounds;
        };

        uniform vec2 u_resolution;
        uniform vec3 u_worldCameraPos;
        uniform mat4 u_projMat;
        uniform mat4 u_viewMat;

        uniform samplerBuffer u_bvhNodeTBO;
        uniform samplerBuffer u_bvhMinBoundsTBO;
        uniform samplerBuffer u_bvhMaxBoundsTBO;
        uniform samplerBuffer u_posTBO;
        uniform samplerBuffer u_normalTBO;
        uniform int u_bvhLeafStartIdx;

        uniform samplerBuffer u_roomPosTBO;
        uniform samplerBuffer u_roomNormalTBO;
        uniform vec3 u_roomFaceColors[5];

        const float INF = 9999999.0;
        const int MATERIAL_TYPE_DIFFUSE = 0;
        const int MATERIAL_TYPE_METAL = 1;


        layout (location = 0) out vec4 fragColor;
        in vec2 v_uv;

        vec3 getBGColor(Ray ray) {
            float a = 0.5 * (ray.dir.y + 1.0);
            return (1.0 - a) * vec3(1.0, 1.0, 1.0) + a * vec3(0.5, 0.7, 1.0);
        }

        float rand(vec2 co) {
            return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
        }

        vec3 randomHemisphereDir(vec3 N) {
            vec2 randomSeed = gl_FragCoord.xy;
            float u = rand(randomSeed);
            float v = rand(randomSeed + vec2(1.0, 1.0));

            float theta = 2.0 * 3.14159265359 * u;
            float phi = acos(2.0 * v - 1.0);

            float x = sin(phi) * cos(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(phi);

            vec3 randomVec = vec3(x, y, z);

            if (dot(randomVec, N) < 0.0) {
                randomVec = -randomVec;
            }

            return normalize(randomVec);
        }

        Hit rayTriangle(Ray ray, Triangle tri) {
            vec3 edgeAB = tri.posB - tri.posA;
            vec3 edgeAC = tri.posC - tri.posA;
            vec3 N = cross(edgeAB, edgeAC);
            vec3 ao = ray.org - tri.posA;
            vec3 dao = cross(ao, ray.dir);

            float determinant = -dot(ray.dir, N);
            float invDet = 1.0 / determinant;

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


        Hit rayBoundingBox(Ray ray, vec3 Bmin, vec3 Bmax)
        {
            Hit hit;

            vec3 invD = 1.0 / ray.dir;
            vec3 t0s = (Bmin - ray.org) * invD;
            vec3 t1s = (Bmax - ray.org) * invD;
            vec3 tsmaller = min(t0s, t1s);
            vec3 tbigger = max(t0s, t1s);

            float tmin = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
            float tmax = min(min(tbigger.x, tbigger.y), tbigger.z);

            if (tmax < 0.0 || tmin > tmax) {
                hit.didHit = false;
                return hit;
            }

            float t = (tmin < 0.0) ? tmax : tmin;
            hit.pos = ray.org + t * ray.dir;
            hit.didHit = true;
            hit.dst = t;

            if (hit.pos.x <= Bmin.x + 1e-4) hit.N = vec3(-1.0, 0.0, 0.0);
            else if (hit.pos.x >= Bmax.x - 1e-4) hit.N = vec3(1.0, 0.0, 0.0);
            else if (hit.pos.y <= Bmin.y + 1e-4) hit.N = vec3(0.0, -1.0, 0.0);
            else if (hit.pos.y >= Bmax.y - 1e-4) hit.N = vec3(0.0, 1.0, 0.0);
            else if (hit.pos.z <= Bmin.z + 1e-4) hit.N = vec3(0.0, 0.0, -1.0);
            else if (hit.pos.z >= Bmax.z - 1e-4) hit.N = vec3(0.0, 0.0, 1.0);

            return hit;
        }

        BVHNode getBVHNode(int i) {
            ivec3 indexInfo = ivec3(texelFetch(u_bvhNodeTBO, i).xyz);
            vec3 minBounds = texelFetch(u_bvhMinBoundsTBO, i).xyz;
            vec3 maxBounds = texelFetch(u_bvhMaxBoundsTBO, i).xyz;

            BVHNode node;
            node.minBounds = minBounds;
            node.maxBounds = maxBounds;
            node.triangleIdx = indexInfo.x;
            node.triangleCnt = indexInfo.y;
            node.nodeIdx = indexInfo.z;

            return node;
        }

        Triangle getTriangle(int i) {
            const int STRIDE = 3;
            Triangle tri;
            tri.posA = texelFetch(u_posTBO, i * STRIDE + 0).xyz;
            tri.posB = texelFetch(u_posTBO, i * STRIDE + 1).xyz;
            tri.posC = texelFetch(u_posTBO, i * STRIDE + 2).xyz;
            tri.NA   = texelFetch(u_normalTBO, i * STRIDE + 0).xyz;
            tri.NB   = texelFetch(u_normalTBO, i * STRIDE + 1).xyz;
            tri.NC   = texelFetch(u_normalTBO, i * STRIDE + 2).xyz;
            return tri;
        }

        Triangle getRoomTriangle(int i) {
            const int STRIDE = 3;
            Triangle tri;
            tri.posA = texelFetch(u_roomPosTBO, i * STRIDE + 0).xyz;
            tri.posB = texelFetch(u_roomPosTBO, i * STRIDE + 1).xyz;
            tri.posC = texelFetch(u_roomPosTBO, i * STRIDE + 2).xyz;
            tri.NA   = texelFetch(u_roomNormalTBO, i * STRIDE + 0).xyz;
            tri.NB   = texelFetch(u_roomNormalTBO, i * STRIDE + 1).xyz;
            tri.NC   = texelFetch(u_roomNormalTBO, i * STRIDE + 2).xyz;
            return tri;
        }


        // rayCollisionTriangleBVH의 스택+반복버전이 필요함(쉐이더에서 리커전 지원X)
        Hit rayCollisionTriangleBVH(Ray ray) {
            Hit result;
            result.didHit = false;
            result.dst = INF;

            int idx = 0;
            BVHNode nodeStack[20];
            nodeStack[idx++] = getBVHNode(1);

            vec3 modelColor = vec3(0.8);

            while (idx > 0) {
                BVHNode current = nodeStack[--idx];
                Hit boxHit = rayBoundingBox(ray, current.minBounds, current.maxBounds);
                if (boxHit.didHit) {
                    if (current.nodeIdx >= u_bvhLeafStartIdx) {
                        for (int triIdx = current.triangleIdx; triIdx < current.triangleIdx + current.triangleCnt; ++triIdx) {
                            Hit triHit = rayTriangle(ray, getTriangle(triIdx));
                            if (triHit.didHit && triHit.dst < result.dst) {
                                result = triHit;
                                result.mat.color = modelColor;
                                result.mat.type = MATERIAL_TYPE_METAL;
                            }
                        }
                    } else {
                        nodeStack[idx++] = getBVHNode(current.nodeIdx * 2);
                        nodeStack[idx++] = getBVHNode(current.nodeIdx * 2 + 1);
                    }
                }
            }

            return result;
        }

        Hit raySphere(Ray ray, vec3 sphereCenter, float sphereRadius) {
            Hit hitInfo;
            hitInfo.didHit = false;
            hitInfo.mat.color = getBGColor(ray);

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

        Hit rayCollisionSphere(Ray ray) {
            Hit closestHit;
            closestHit.didHit = false;
            closestHit.dst = INF;
            closestHit.mat.color = getBGColor(ray);

            //spheres
            const int numSphere = 3;
            Sphere spheres[3];

            spheres[0].pos = vec3(7, 1, -0.5);
            spheres[0].r = 4;
            spheres[0].mat.color = vec3(0.9);
            spheres[0].mat.type = MATERIAL_TYPE_METAL;

            spheres[1].pos = vec3(-7, 1, -0.5);
            spheres[1].r = 4;
            spheres[1].mat.color = vec3(0.9);
            spheres[1].mat.type = MATERIAL_TYPE_METAL;

            spheres[2].pos = vec3(0, 12.0, -0.5);
            spheres[2].r = 4;
            spheres[2].mat.color = vec3(0.9);
            spheres[2].mat.type = MATERIAL_TYPE_METAL;

            for (int i = 0; i < numSphere; ++i) {
                Sphere sphere = spheres[i];
                Hit hit = raySphere(ray, sphere.pos, sphere.r);

                if (hit.didHit && hit.dst < closestHit.dst) {
                    closestHit = hit;
                    closestHit.mat = sphere.mat;
                }
            }

            //bboxes
            const int numAABB = 3;
            AABB aabbs[3];

            aabbs[0].maxBounds = vec3(16, 10, 8);
            aabbs[0].minBounds = vec3(15, -10, -8);
            aabbs[0].mat.color = vec3(0.8, 0.0, 0.0);
            aabbs[0].mat.type = MATERIAL_TYPE_DIFFUSE;

            aabbs[1].maxBounds = vec3(-16, 10, 8);
            aabbs[1].minBounds = vec3(-15, -10, -8);
            aabbs[1].mat.color = vec3(0.0, 0.0, 0.8);
            aabbs[1].mat.type = MATERIAL_TYPE_DIFFUSE;

            aabbs[2].maxBounds = vec3(16, -10,   8);
            aabbs[2].minBounds = vec3(-16, -11, -8);
            aabbs[2].mat.color = vec3(0.0, 0.8, 0.0);
            aabbs[2].mat.type = MATERIAL_TYPE_DIFFUSE;

            for (int i = 0; i < numAABB; ++i) {
                AABB aabb = aabbs[i];
                Hit hit = rayBoundingBox(ray, aabb.minBounds, aabb.maxBounds);
                if (hit.didHit && hit.dst < closestHit.dst) {
                    closestHit = hit;
                    closestHit.mat = aabb.mat;
                }
            }

            return closestHit;
        }


        Hit rayCollision(Ray ray) {
            Hit resultModel = rayCollisionTriangleBVH(ray);
            Hit resultSphere = rayCollisionSphere(ray);
            if (resultModel.dst < resultSphere.dst)
                return resultModel;
            else if (resultModel.dst > resultSphere.dst)
                return resultSphere;
            else
                return resultModel;
        }

        vec3 rayTrace(Ray ray) {
            int MAX_BOUNCE = 5;

            vec3 incomingL = vec3(0);
            vec3 rayColor = vec3(1);

            bool hitted = false;

            for (int i = 0; i < MAX_BOUNCE; ++i) {
                Hit hit = rayCollision(ray);

                if (hit.didHit && hit.mat.type == MATERIAL_TYPE_METAL) {
                    vec3 emittedL = vec3(1.0); // vec3 emittedL = mat.emissionColor * mat.emissionStrength;
                    float s = dot(hit.N, -ray.dir);

                    vec3 specDir = reflect(ray.dir, hit.N);
                    ray.org = hit.pos;
                    ray.dir = specDir;

                    rayColor *= hit.mat.color;
                    incomingL += emittedL * rayColor;

                    hitted = true;

                } else if (hit.didHit && hit.mat.type == MATERIAL_TYPE_DIFFUSE) {
                    incomingL = hit.mat.color * rayColor;
                } else {
                    incomingL = getBGColor(ray) * rayColor;
                    break;
                }
            }

            return incomingL;
        }

        void main() {
            vec4 tmp = texelFetch(u_roomPosTBO, 0);
            vec4 tmp2 = texelFetch(u_roomNormalTBO, 1);
            float tmp3 = (u_resolution.x / u_resolution.y);

            vec2 uv = v_uv * 2.0 - 1.0;

            vec4 rayClip = vec4(uv, -1.0, 1.0);
            vec4 rayEye = inverse(u_projMat) * rayClip;
            rayEye = vec4(rayEye.xy, -1.0, 0.0);
            vec3 worldRay = normalize((inverse(u_viewMat) * rayEye).xyz);

            Ray ray;
            ray.org = u_worldCameraPos;
            ray.dir = worldRay;
            ray.invDir = 1.0 / worldRay;

            float RAY_SAMPLE_CNT = 2.0;
            vec3 total = vec3(0.0);
            for (float i = 0; i < RAY_SAMPLE_CNT; i += 1.0) {
                total += rayTrace(ray);
            }

            fragColor = vec4(total / RAY_SAMPLE_CNT, 1.0);
        }
);

BVHRayTraceShader::BVHRayTraceShader() {
    this->load();
    basicUniformLoc();

    //TBO
    _bvhNodeTBOLoc = glGetUniformLocation(_programID, "u_bvhNodeTBO");
    _bvhMinBoundsTBOLoc = glGetUniformLocation(_programID, "u_bvhMinBoundsTBO");
    _bvhMaxBoundsTBOLoc = glGetUniformLocation(_programID, "u_bvhMaxBoundsTBO");
    _bvhLeafStartIdxLoc = glGetUniformLocation(_programID, "u_bvhLeafStartIdx");
    _posTBOLoc = glGetUniformLocation(_programID, "u_posTBO");
    _normalTBOLoc = glGetUniformLocation(_programID, "u_normalTBO");

    _roomPosTBOLoc = glGetUniformLocation(_programID, "u_roomPosTBO");
    _roomNormalTBOLoc = glGetUniformLocation(_programID, "u_roomNormalTBO");

    _cameraPosUniformLoc = glGetUniformLocation(_programID, "u_worldCameraPos");
    _resolutionUnifromLoc = glGetUniformLocation(_programID, "u_resolution");

    _roomFaceColorsLoc = glGetUniformLocation(_programID, "u_roomFaceColors");
}

bool BVHRayTraceShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexBVHRayTrace);
    string fShader = string("#version 330 core \n") + string(fragmentBVHRayTrace);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);
    return true;
}

void BVHRayTraceShader::useProgram() {

    glUseProgram(_programID);

    assert(_bvhNodeTBOLoc != -1);
    glUniform1i(_bvhNodeTBOLoc, 0);

    assert(_bvhMinBoundsTBOLoc != -1);
    glUniform1i(_bvhMinBoundsTBOLoc, 1);

    assert(_bvhMaxBoundsTBOLoc != -1);
    glUniform1i(_bvhMaxBoundsTBOLoc, 2);

    assert(_posTBOLoc != -1);
    glUniform1i(_posTBOLoc, 3);

    assert(_normalTBOLoc != -1);
    glUniform1i(_normalTBOLoc, 4);

    assert(_roomPosTBOLoc != -1);
    glUniform1i(_roomPosTBOLoc, 5);

    assert(_roomNormalTBOLoc != -1);
    glUniform1i(_roomNormalTBOLoc, 6);

}