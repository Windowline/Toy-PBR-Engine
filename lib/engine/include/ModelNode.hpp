#ifndef TOYRENDERER_MODELNODE_HPP
#define TOYRENDERER_MODELNODE_HPP
#include "Matrix.hpp"
#include <memory>

#include "MeshBasic.h"

using namespace std;

class Scene;

class ModelNode {

public:
    ModelNode(Scene *scene, shared_ptr<MeshBasic> mesh, mat4 localTransform);

    void setLocalInstanceTransforms(vector<mat4>&& localInstanceTransform);

    virtual ~ModelNode() {}

    void addChild(shared_ptr<ModelNode> node);

    void transformUpdate();

    void setLocalTransform(mat4 localTransform);

    void render();

    bool isInstancing() const {
        return _localInstanceTransforms.size() >= 1;
    }

    int instanceCount() const {
        return _localInstanceTransforms.size();
    }

    const mat4& worldTransform() const {
        return _worldTransform;
    }

    const vector<mat4>& worldInstanceTransforms() const {
        return _worldInstanceTransforms;
    }

    const vector<mat4>& worldInstanceNormalTransforms() const {
        return _worldInstanceNormalTransforms;
    }

    vector<mat4> worldInstanceTransformsCombind(const mat4& other) const {
        vector<mat4> ret;
        for (const auto& m : _worldInstanceTransforms)
            ret.push_back(m * other);
        return ret;
    }

    const vector<shared_ptr<ModelNode>>& children() const {
        return _children;
    }

    shared_ptr<MeshBasic> const& mesh() const {
        return _mesh;
    }

    string meshName() const {
        return _mesh->name();
    }

    void setEnabled(bool v) {
        _enabled = v;
    }

private:
    mat4 _localBasicTransform;
    mat4 _worldTransform;
    vector<mat4> _localInstanceTransforms;
    vector<mat4> _worldInstanceTransforms;
    vector<mat4> _worldInstanceNormalTransforms;

    Scene* _scene;
    ModelNode* _parent;

    shared_ptr<MeshBasic> _mesh;
    vector<shared_ptr<ModelNode>> _children;

    bool _enabled = true;
};

#endif //TOYRENDERER_MODELNODE_HPP
