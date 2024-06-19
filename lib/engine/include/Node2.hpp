#ifndef TOYRENDERER_NODE2_HPP
#define TOYRENDERER_NODE2_HPP
#include "Matrix.hpp"
#include <memory>

class BasicMeshInterface;
class Scene;

class Node2 {

public:
    Node2(Scene *scene, std::shared_ptr<BasicMeshInterface> mesh, mat4 localTransform);

    virtual ~Node2();

    void addChild(std::shared_ptr<Node2> node);

    void transformUpdate();

    void setLocalTransform(mat4 localTransform);

    void render();

    const mat4 worldTransform() const {
        return _worldTransform;
    }

    const std::vector<std::shared_ptr<Node2>>& children() const {
        return _children;
    }

    std::shared_ptr<BasicMeshInterface> const& mesh() const {
        return _mesh;
    }

private:
    mat4 _localTransform;
    mat4 _worldTransform;
    Scene* _scene;
    Node2* _parent;

    std::shared_ptr<BasicMeshInterface> _mesh;
    std::vector<std::shared_ptr<Node2>> _children;
};

#endif //TOYRENDERER_NODE2_HPP
