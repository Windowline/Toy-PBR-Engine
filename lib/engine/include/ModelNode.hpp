#ifndef TOYRENDERER_MODELNODE_HPP
#define TOYRENDERER_MODELNODE_HPP
#include "Matrix.hpp"
#include <memory>

#include "MeshBasic.h"

class Scene;

class ModelNode {

public:
    ModelNode(Scene *scene, std::shared_ptr<MeshBasic> mesh, mat4 localTransform);

    virtual ~ModelNode() {}

    void addChild(std::shared_ptr<ModelNode> node);

    void transformUpdate();

    void setLocalTransform(mat4 localTransform);

    void render();

    const mat4& worldTransform() const {
        return _worldTransform;
    }

    const std::vector<std::shared_ptr<ModelNode>>& children() const {
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
    Scene* _scene;
    ModelNode* _parent;

    std::shared_ptr<MeshBasic> _mesh;
    std::vector<std::shared_ptr<ModelNode>> _children;

    bool _enabled = true;
};

#endif //TOYRENDERER_MODELNODE_HPP
