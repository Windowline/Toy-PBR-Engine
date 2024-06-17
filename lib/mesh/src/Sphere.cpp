
#include "Sphere.hpp"
//#include "TargetConditionals.h"


const int SECTOR_COUNT = 50;
const int STACK_COUNT = 50;

//#if TARGET_OS_SIMULATOR
//    const int SECTOR_COUNT = 10;
//    const int STACK_COUNT = 10;
//#else
//    const int SECTOR_COUNT = 50;
//    const int STACK_COUNT = 50;
//#endif

Sphere::Sphere(float radius, vec3 color) : _radius(radius), _color(std::move(color)) {
    _rayCheckType = Mesh::RayCheckType::Sphere;
    buildMesh();
}

void Sphere::buildMesh() {

    float lengthInv = 1.0f / _radius;
    
    float sectorStep = 2 * PI / SECTOR_COUNT;
    float stackStep = PI / STACK_COUNT;
    float sectorAngle, stackAngle;
    

    for(int i = 0; i <= STACK_COUNT; ++i) {
        stackAngle = PI / 2 - i * stackStep;
        float xy = _radius * cos(stackAngle);
        float z = _radius * sin(stackAngle);

        for(int j = 0; j <= SECTOR_COUNT; ++j) {
            sectorAngle = j * sectorStep;
            float x = xy * cos(sectorAngle);   // r * cos(u) * cos(v)
            float y = xy * sin(sectorAngle);   // r * cos(u) * sin(v)
            
            _vertices.emplace_back(vec3(x, y, z));
            _normals.emplace_back(vec3(x * lengthInv, y * lengthInv, z * lengthInv));
            _colors.emplace_back(_color);
        }
    }
    
    
    std::vector<unsigned int> indices;

    for(int i = 0; i < STACK_COUNT; ++i) {
        float k1 = i * (SECTOR_COUNT + 1);     //current stack
        float k2 = k1 + SECTOR_COUNT + 1;      //next stack
        for(int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2) {
            // k1 => k2 => k1+1
            if(i != 0) {
                indices.emplace_back(k1);
                indices.emplace_back(k2);
                indices.emplace_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (STACK_COUNT-1)) {
                indices.emplace_back(k1 + 1);
                indices.emplace_back(k2);
                indices.emplace_back(k2 + 1);
            }
        }
    }
    
    _indices = std::move(indices);
}


//버텍스 수 떄문인지 simulator에서 문제가 발생하여 보류.
//void Sphere::buildMeshTest() {
//
//    constexpr float LAST_LAT_DEGREE = 359.f;
//    constexpr float LAST_LAT_DEGREE_ADJUST = 359.999999f;
//    const int maxLat = 360;
//    const int maxLon = 180;
//
//    auto toCartesian = [](float lat, float lon, float r) -> vec3 {
//        float latRadian = lat * M_PI / 180.0;
//        float lonRadian = lon * M_PI / 180.0;
//        float sinLon = sin(lonRadian);
//        float sinLat = sin(latRadian);
//        float cosLon = cos(lonRadian);
//        float cosLat = cos(latRadian);
//        return vec3(r * sinLon * cosLat, r * cosLon, r * sinLon * sinLat);
//    };
//
//    std::vector<vec3> vertices;
//    vertices.reserve(maxLat * maxLon);
//
//    std::vector<vec3> normals;
//    normals.reserve(maxLat * maxLon);
//
//    std::vector<vec3> colors;
//    colors.reserve(maxLat * maxLon);
//
//    for (float lat = 0.0; lat < maxLat; lat += 1.f) {
//        for (float lon = 0.5; lon < maxLon; lon += 1.f) {
//            if (lat == LAST_LAT_DEGREE) {
//                lat = LAST_LAT_DEGREE_ADJUST;
//            }
//
//            vec3 pos = toCartesian(lat, lon, _radius);
//
//            vertices.emplace_back(pos);
//            normals.emplace_back(pos.normalized());
//            colors.emplace_back(_color);
//
//        }
//    }
//
//
//    std::vector<unsigned int> indices;
//    indices.reserve((maxLat - 1) * (maxLon - 1) * 6 + 6 * (maxLat - 1));
//
//    for (int i = 0; i < maxLat; ++i) {
//        for (int j = 0; j < maxLon; ++j) {
//            int next = (i + 1);
//            indices.emplace_back(i * maxLon + j);         // (i, j)
//            indices.emplace_back(next * maxLon + j);      // (i+1, j)
//            indices.emplace_back(i * maxLon + j + 1);     // (i, j+1)
//            indices.emplace_back(next * maxLon + j);      //(i+1, j)
//            indices.emplace_back(next * maxLon + j + 1);  // (i+1, j+1)
//            indices.emplace_back(i * maxLon + j + 1);     // (i, j+1)
//        }
//    }
//
//    for (int i = 0; i < maxLat - 1; ++i) {
//        indices.emplace_back(i * maxLon + maxLon - 1);
//        indices.emplace_back((i + 1) * maxLon + maxLon - 1);
//        indices.emplace_back(i * maxLon);
//        indices.emplace_back((i + 1) * maxLon + maxLon - 1);
//        indices.emplace_back((i + 1) * maxLon);
//        indices.emplace_back(i * maxLon);
//    }
//
//    _vertices = std::move(vertices);
//    _normals = std::move(normals);
//    _colors = std::move(colors);
//    _indices = std::move(indices);
//}
