
#ifndef Mesh_hpp
#define Mesh_hpp
#include "Vector.hpp"
#include "Matrix.hpp"
#include "BasicLightingShader.hpp"
#include <vector>

/**
 * 렌더링 할 최소 단위입니다.
 * 정점, 법선, 색상, uv, 인덱스를 갖습니다.
 */
class Mesh
{
    
public:
    
    enum class RayCheckType : int {
        Sphere = 0,
        Box = 1,
        Plane = 2,
        Unknown = 3
    };
    
    struct BoundingSphere { //origin 0, 0, 0
        float radius;
    };
    
    struct BoundingBox {
        vec3 minPos;
        vec3 maxPos;
    };
    
    Mesh();
    
    virtual ~Mesh();
    
    void purgeGLData();
    
    void render(std::shared_ptr<ShaderBase> const& shader);
    
    void upload();
    
    void bindBuffer(const std::shared_ptr<ShaderBase>& shader) const;
    
    bool isRenderable() const;
    
    RayCheckType rayCheckType() const {
        return _rayCheckType;
    }
    
    float boundingSphereRadius() const {
        return _boundingSphere.radius;
    }
    
    const BoundingBox& boundingBox() const {
        return _boundingBox;
    }
    
protected:
    virtual void buildMesh();
    
    void buildBoundingStructure();
    
    
    std::vector<unsigned int> _indices;
    std::vector<vec3> _vertices;
    std::vector<vec3> _normals;
    std::vector<vec3> _colors;
    std::vector<vec2> _uvs;

    unsigned long _indSize = 0;
    unsigned long _vertexSize = 0;
    unsigned long _normalSize = 0;
    unsigned long _colorSize = 0;
    unsigned long _uvSize = 0;
    
    
    GLuint _indexBuffer = 0;
    GLuint _positionBuffer = 0;
    GLuint _uvBuffer = 0;
    GLuint _normalBuffer = 0;
    GLuint _colorBuffer = 0;
    
    bool _isUploaded = false;

    //hit test
    RayCheckType _rayCheckType;
    BoundingSphere _boundingSphere;
    BoundingBox _boundingBox;
};




#endif /* Mesh_hpp */
