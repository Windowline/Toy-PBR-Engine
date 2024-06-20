#ifndef TOYRENDERER_NODE_HPP
#define TOYRENDERER_NODE_HPP
#include "Matrix.hpp"
#include <memory>

class BasicMeshInterface;
class Scene;

class Node {

public:
    Node(Scene *scene, std::shared_ptr<BasicMeshInterface> mesh, mat4 localTransform);

    virtual ~Node() {}

    void addChild(std::shared_ptr<Node> node);

    void transformUpdate();

    void setLocalTransform(mat4 localTransform);

    void render();

    const mat4& worldTransform() const {
        return _worldTransform;
    }

    const std::vector<std::shared_ptr<Node>>& children() const {
        return _children;
    }

    std::shared_ptr<BasicMeshInterface> const& mesh() const {
        return _mesh;
    }

private:
    mat4 _localTransform;
    mat4 _worldTransform;
    Scene* _scene;
    Node* _parent;

    std::shared_ptr<BasicMeshInterface> _mesh;
    std::vector<std::shared_ptr<Node>> _children;
};

#endif //TOYRENDERER_NODE_HPP
