
#include "../include/Mesh.hpp"
#include "GLUtilGeometry.hpp"

Mesh::Mesh() {
   // buildMesh();
}

Mesh::~Mesh() {
    purgeGLData();
}

void Mesh::buildMesh() {
    printf("do iml\n");
}

void Mesh::purgeGLData() {
    
    if (!_isUploaded)
        return;

    if (_positionBuffer != INVALID_GL_ID) {
        glDeleteBuffers(1, &_positionBuffer);
        _positionBuffer = INVALID_GL_ID;
    }

    if (_uvBuffer != INVALID_GL_ID) {
        glDeleteBuffers(1, &_uvBuffer);
        _uvBuffer = INVALID_GL_ID;
    }

    if (_indexBuffer != INVALID_GL_ID) {
        glDeleteBuffers(1, &_indexBuffer);
        _indexBuffer = INVALID_GL_ID;
    }

    if (_normalBuffer != INVALID_GL_ID) {
        glDeleteBuffers(1, &_normalBuffer);
        _normalBuffer = INVALID_GL_ID;
    }
    
    if (_colorBuffer != INVALID_GL_ID) {
        glDeleteBuffers(1, &_colorBuffer);
        _colorBuffer = INVALID_GL_ID;
    }
    
    _indSize = 0;
    _isUploaded = false;
}


void Mesh::upload() {
    
    _indSize = _indices.size();
    _vertexSize = _vertices.size();
    _uvSize = _uvs.size();
    _normalSize = _normals.size();
    _colorSize = _colors.size();
    
    
    if (!_vertices.empty()) {
        glGenBuffers(1, &_positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(vec3), _vertices.data(),
                     GL_STATIC_DRAW);
        GLUtil::GL_ERROR_LOG();
        std::vector<vec3>().swap(_vertices);
    }

    if (!_uvs.empty()) {
        glGenBuffers(1, &_uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(vec2), _uvs.data(), GL_STATIC_DRAW);
        GLUtil::GL_ERROR_LOG();
        std::vector<vec2>().swap(_uvs);
    }
    
    if (!_normals.empty()) {
        glGenBuffers(1, &_normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(vec3), _normals.data(),
                     GL_STATIC_DRAW);
        GLUtil::GL_ERROR_LOG();
        std::vector<vec3>().swap(_normals);
    }
    
    if (!_colors.empty()) {
        glGenBuffers(1, &_colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, _colors.size() * sizeof(vec3), _colors.data(),
                     GL_STATIC_DRAW);
        GLUtil::GL_ERROR_LOG();
        std::vector<vec3>().swap(_colors);
    }
    
    
    if (!_indices.empty()) {
        glGenBuffers(1, &_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(),
                     GL_STATIC_DRAW);
        GLUtil::GL_ERROR_LOG();
        std::vector<unsigned int>().swap(_indices);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _isUploaded = true;
}


void Mesh::bindBuffer(const std::shared_ptr<ShaderBase>& shader) const {
    
    if(_positionBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, _positionBuffer);
        shader->positionAttribPointer(0, 3);
    }
    
//    if (_uvBuffer != 0) {
//        glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
//        shader->texCoordAttribPointer(0, 2);
//    }

    if (_normalBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, _normalBuffer);
        shader->normalAttribPointer(0, 3);
    }
    
    if (_colorBuffer != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
        shader->colorAttribPointer(0, 3);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
}



void Mesh::render(std::shared_ptr<ShaderBase> const& shader) {
    
    if (!_isUploaded) {
        upload();
    }
    
    if (!isRenderable()) {
        return;
    }
    
    bindBuffer(shader);
    glDrawElements(GL_TRIANGLES, (GLsizei)_indSize, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
}

bool Mesh::isRenderable() const {
  
    if (_vertexSize > 0 && _positionBuffer == INVALID_GL_ID)
        return false;
    if (_uvSize > 0 && _uvBuffer == INVALID_GL_ID)
        return false;
    if (_normalSize > 0 && _normalBuffer == INVALID_GL_ID)
        return false;
    if (_colorSize > 0 && _colorBuffer == INVALID_GL_ID)
        return false;
    if (_indSize > 0 && _indexBuffer == INVALID_GL_ID)
        return false;
    
    return true;
}

void Mesh::buildBoundingStructure() {
    
    float maxLen = 0;
    float maxFloat = std::numeric_limits<float>::max();
    float minFloat = std::numeric_limits<float>::lowest();
    
    vec3 minPos = vec3(maxFloat, maxFloat, maxFloat);
    vec3 maxPos = vec3(minFloat, minFloat, minFloat);
    
    
    for (auto& v : _vertices) {
        minPos.x = fmin(minPos.x, v.x);
        minPos.y = fmin(minPos.y, v.y);
        minPos.z = fmin(minPos.z, v.z);
        maxPos.x = fmax(maxPos.x, v.x);
        maxPos.y = fmax(maxPos.y, v.y);
        maxPos.z = fmax(maxPos.z, v.z);
        
        maxLen = fmax(maxLen, v.length());
    }
    
    _boundingBox.maxPos = maxPos;
    _boundingBox.minPos = minPos;
    
    _boundingSphere.radius = maxLen;
    
}




