#ifndef Node_hpp
#define Node_hpp

#include "ShaderManager.hpp"
#include "Matrix.hpp"
#include <vector>
#include <memory>

class Ray;
class GBufferShader;
class BasicLightingShader;
class ShaderBase;
class ShadowDepthShader;;
class Mesh;
class Scene;
class Mesh;

/*
 * 메시, 트랜스폼 정보를 가지고 있습니다.
 * 트랜스폼 업데이트를 위해, 재귀 계층형 구조를 갖습니다.
 */
class Node {

public:
    Node(Scene* scene, std::shared_ptr<Mesh> mesh, mat4 localTransform);

    virtual ~Node();

    void addChild(std::shared_ptr<Node> node);

    void transformUpdate();

    void setLocalTransform(mat4 localTransform);

    void render();

    const mat4 worldTransform() const {
        return _worldTransform;
    }

    const std::vector<std::shared_ptr<Node>>& children() const {
        return _children;
    }

    void setShaderType(eShaderProgram shaderType) {
        _shaderType = shaderType;
    }

    bool hitTest(Ray ray, vec3& outPos);

    vec3 worldNormalAt(vec3 point);

    std::shared_ptr<Mesh> const& mesh() const {
        return _mesh;
    }

private:

    void setShadowDepthShader(std::shared_ptr<ShadowDepthShader> shader);

    void setGBufferShader(std::shared_ptr<GBufferShader> shader);

    void setBasicPhongShader(std::shared_ptr<BasicLightingShader> shader);

    void setSceneLightCommonProperty(std::shared_ptr<BasicLightingShader>const & shader);

    mat4 _localTransform;

    mat4 _worldTransform;

    Scene* _scene;

    Node* _parent;

    std::shared_ptr<Mesh> _mesh;

    std::vector<std::shared_ptr<Node>> _children;

    eShaderProgram _shaderType;

};



#endif /* Node_hpp */
