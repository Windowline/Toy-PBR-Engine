#ifndef TOYRENDERER_NODE_HPP
#define TOYRENDERER_NODE_HPP
#include "Matrix.hpp"
#include <memory>

#include "MeshBasic.h"

class PBRScene;

class Node {

public:
    Node(PBRScene *scene, std::shared_ptr<MeshBasic> mesh, mat4 localTransform);

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

    std::shared_ptr<MeshBasic> const& mesh() const {
        return _mesh;
    }

    std::string meshName() const {
        return _mesh->name();
    }

    void setEnabled(bool v) {
        _enabled = v;
    }

private:
    mat4 _localTransform;
    mat4 _worldTransform;
    PBRScene* _scene;
    Node* _parent;

    std::shared_ptr<MeshBasic> _mesh;
    std::vector<std::shared_ptr<Node>> _children;

    bool _enabled = true;
};

#endif //TOYRENDERER_NODE_HPP
