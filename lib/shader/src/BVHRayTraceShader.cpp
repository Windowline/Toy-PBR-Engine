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

        struct BVHNode {
            int triangleIndex;
            int triangleCount;
            int childIndex;
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
        uniform samplerBuffer u_bvhTriangleTBO;
        uniform int u_triangleSize;

        float INF = 9999999.0;

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

        bool rayBoundingBox(Ray ray, vec3 boundsMin, vec3 boundsMax) {
            vec3 invDir = 1.0 / ray.dir;
            vec3 t0s = (boundsMin - ray.org) * invDir;
            vec3 t1s = (boundsMax - ray.org) * invDir;
            vec3 tsmaller = min(t0s, t1s);
            vec3 tbigger = max(t0s, t1s);

            float tmin = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
            float tmax = min(min(tbigger.x, tbigger.y), tbigger.z);
            return tmax >= max(tmin, 0.0);
        }

        BVHNode getBVHNode(int i) {
            ivec3 indexInfo = ivec3(texelFetch(u_bvhNodeTBO, i).xyz);
            vec3 minBounds = texelFetch(u_bvhMinBoundsTBO, i).xyz;
            vec3 maxBounds = texelFetch(u_bvhMaxBoundsTBO, i).xyz;

            BVHNode node;
            node.minBounds = minBounds;
            node.maxBounds = maxBounds;
            node.triangleIndex = indexInfo.x;
            node.triangleCount = indexInfo.y;
            node.childIndex = indexInfo.z;

            return node;
        }

        Triangle getTriangle(int i) {
            const int STRIDE = 6;
            Triangle tri;
            tri.posA = texelFetch(u_bvhTriangleTBO, i * STRIDE + 0).xyz;
            tri.posB = texelFetch(u_bvhTriangleTBO, i * STRIDE + 1).xyz;
            tri.posC = texelFetch(u_bvhTriangleTBO, i * STRIDE + 2).xyz;
            tri.NA   = texelFetch(u_bvhTriangleTBO, i * STRIDE + 3).xyz;
            tri.NB   = texelFetch(u_bvhTriangleTBO, i * STRIDE + 4).xyz;
            tri.NC   = texelFetch(u_bvhTriangleTBO, i * STRIDE + 5).xyz;
            return tri;
        }

        // RayTriangleTestBVH의 스택+반복버전이 필요함(쉐이더에서 리커전 지원X)
        Hit rayTriangleTestBVH(Ray ray) {
            int idx = 0;
            BVHNode nodeStack[20];
            nodeStack[idx++] = getBVHNode(0);

            Hit result;
            result.didHit = false;
            result.dst = INF;

            while (idx > 0) {
                BVHNode node = nodeStack[--idx];
                if (rayBoundingBox(ray, node.minBounds, node.maxBounds)) {
                    if (node.childIndex == 0) { //leaf
                        for (int i = node.triangleIndex; i < node.triangleIndex + node.triangleCount; ++i) {
                            Hit triHit = rayTriangle(ray, getTriangle(i));
                            if (triHit.dst < result.dst)
                                result = triHit;
                        }
                    } else {
                        nodeStack[idx++] = getBVHNode(node.childIndex + 1);
                        nodeStack[idx++] = getBVHNode(node.childIndex + 0);
                    }
                }
            }

            return result;
        }

        void main() {
            ivec3 tmp1 = ivec3(texelFetch(u_bvhNodeTBO, 0).xyz);
            vec3 tmp2 = texelFetch(u_bvhMinBoundsTBO, 0).xyz;
            vec3 tmp3 = texelFetch(u_bvhMaxBoundsTBO, 0).xyz;
            vec3 tmp4 = texelFetch(u_bvhTriangleTBO, 0).xyz;
            int tmp5 = u_triangleSize;

            vec2 uv = v_uv * 2.0 - 1.0;
            uv.x *= (u_resolution.x / u_resolution.y);

            vec4 rayClip = vec4(uv, -1.0, 1.0);
            vec4 rayEye = inverse(u_projMat) * rayClip;
            rayEye = vec4(rayEye.xy, -1.0, 0.0);
            vec3 rayWorld = normalize((inverse(u_viewMat) * rayEye).xyz);
            Ray ray;
            ray.org = u_worldCameraPos;
            ray.dir = rayWorld;

            fragColor = vec4(1.0);
        }
);

BVHRayTraceShader::BVHRayTraceShader() {
    this->load();
    basicUniformLoc();

    //TBO
    _bvhNodeTBOLoc = glGetUniformLocation(_programID, "u_bvhNodeTBO");
    _bvhMinBoundsTBOLoc = glGetUniformLocation(_programID, "u_bvhMinBoundsTBO");
    _bvhMaxBoundsTBOLoc = glGetUniformLocation(_programID, "u_bvhMaxBoundsTBO");
    _bvhTriangleTBOLoc = glGetUniformLocation(_programID, "u_bvhTriangleTBO");

    _cameraPosUniformLoc = glGetUniformLocation(_programID, "u_worldCameraPos");
    _resolutionUnifromLoc = glGetUniformLocation(_programID, "u_resolution");
    _triangleSizeLoc = glGetUniformLocation(_programID, "u_triangleSize");
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

    assert(_bvhTriangleTBOLoc != -1);
    glUniform1i(_bvhTriangleTBOLoc, 3);
}