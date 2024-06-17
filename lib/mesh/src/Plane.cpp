
#include "../include/Plane.hpp"

Plane::Plane(vec2 size) : _size(std::move(size)) {
    buildMesh();
    _rayCheckType = Mesh::RayCheckType::Plane;
}

//~Plane

void Plane::buildMesh() {
    
    std::vector<vec3> vertices;
    
    vertices.emplace_back(vec3(-_size.x / 2.f, -_size.y / 2.f, 0.f)); //lb
    vertices.emplace_back(vec3(_size.x / 2.f, -_size.y / 2.f, 0.f)); //rb
    vertices.emplace_back(vec3(-_size.x / 2.f, _size.y / 2.f, 0.f)); //lt
    vertices.emplace_back(vec3(_size.x / 2.f, _size.y / 2.f, 0.f)); //rb
    
    
    std::vector<unsigned int> indices {
        0, 1, 2,
        1, 3, 2
    };
    
    
    _vertices = std::move(vertices);
    _indices = std::move(indices);
}




