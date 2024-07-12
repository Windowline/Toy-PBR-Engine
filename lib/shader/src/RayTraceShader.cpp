#include "RayTraceShader.hpp"

const char* vertexRayTrace = R(
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_texCoord;
        out vec2 v_uv;

        void main() {
            v_uv = a_texCoord;
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
);

const char* fragmentRayTrace = R(
        uniform vec2 u_resolution;
        uniform mat4 u_cameraLocalToWorldMat;
        uniform vec3 u_worldCameraPos;


        struct Ray {
            vec3 org;
            vec3 dir;
        };

        struct Hit {
            bool didHit;
            float dst;
            vec3 pos;
            vec3 normal;
        };

        Hit RaySphere(Ray ray, vec3 sphereCenter, float sphereRadius) {
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
                    hitInfo.normal = normalize(hitInfo.pos - sphereCenter);
                }
            }

            return hitInfo;
        }


        layout (location = 0) out vec4 fragColor;
        in vec2 v_uv;

        void main() {
            vec3 tmp = u_worldCameraPos;
            mat4 tmp2 = u_cameraLocalToWorldMat;
            vec2 tmp3 = u_resolution;

            vec2 uv = v_uv * 2.0 - 1.0;
            float aspect = u_resolution.x / u_resolution.y;
            uv.x *= aspect;

            Ray ray;
//            ray.org = u_worldCameraPos;
//            ray.dir = normalize(vec3(uv, -1.0));
            ray.org = vec3(0.0, 0.0, 50.0);
            ray.dir = normalize(vec3(uv, -1.0));

            Hit hitInfo = RaySphere(ray, vec3(0.0), 25.0);

            if (hitInfo.didHit) {
                fragColor = vec4(0.0, 1.0, 0.0, 1.0);
            } else {
                fragColor = vec4(0.0, 0.0, 0.0, 1.0);
            }


//            vec2 uv = (gl_FragCoord.xy / vec2(800, 600)) * 2.0 - 1.0;
//            uv.x *= 800.0 / 600.0;
//
//            vec3 rayOrigin = cameraPosition;
//            vec3 rayDirection = normalize(vec3(uv, -1.0));
//
//            vec3 color = traceRay(rayOrigin, rayDirection, maxReflections);
//
//            FragColor = vec4(color, 1.0);



//            vec3 tmp = u_worldCameraPos;
//            mat4 tmp2 = u_cameraLocalToWorldMat;
//
//            vec3 viewParams = vec3(1.0);
//            vec3 viewPosLocal = vec3(uv - vec2(0.5), 1.0) * viewParams;
////            vec3 viewPos = (u_cameraLocalToWorldMat * vec4(viewPosLocal, 1.0)).xyz;
////            vec3 viewPos = u_worldCameraPos;
//            vec3 viewPos = vec3(0.0);
//
//            Ray ray;
//            ray.org = u_worldCameraPos;
//            ray.dir = normalize(viewPos - ray.org);
//
//            Hit hitInfo = RaySphere(ray, vec3(0.0), 1000.0);
//
////            fragColor = vec4(viewPosLocal, 1.0);
//
//
//            if (hitInfo.didHit) {
//                fragColor = vec4(0.0, 1.0, 0.0, 1.0);
//            } else {
//                fragColor = vec4(0.0, 0.0, 0.0, 1.0);
//            }
        }
);

RayTraceShader::RayTraceShader() {
    this->load();
    basicUniformLoc();

    _cameraPosUniformLoc = glGetUniformLocation(_programID, "u_worldCameraPos");
    _cameraLocalToWorldMatUniformLoc = glGetUniformLocation(_programID, "u_cameraLocalToWorldMat");
    _resolutionUnifromLoc = glGetUniformLocation(_programID, "u_resolution");
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
}