#include "ModelNode.hpp"
#include "MeshBasic.h"

using namespace std;

ModelNode::ModelNode(Scene *scene, shared_ptr<MeshBasic> mesh, mat4 localTransform)
        : _scene(scene), _mesh(std::move(mesh)), _localBasicTransform(std::move(localTransform)),
          _parent(nullptr) {

}

void ModelNode::setLocalInstanceTransforms(std::vector<mat4>&& localInstanceTransform) {
    _localInstanceTransforms = std::move(localInstanceTransform);
}

void ModelNode::addChild(std::shared_ptr<ModelNode> node) {
    _children.emplace_back(node);
    node->_parent = this;
}

void ModelNode::transformUpdate() {
    _worldTransform = _parent ? _parent->worldTransform() * _localBasicTransform : _localBasicTransform;

    _worldInstanceTransforms.clear();
    _worldInstanceNormalTransforms.clear();

    for (int i = 0; i < _localInstanceTransforms.size(); ++i) {
        if (_parent)
            _worldInstanceTransforms.push_back(_parent->worldTransform() * _localBasicTransform * _localInstanceTransforms[i]);
        else
            _worldInstanceTransforms.push_back(_localBasicTransform * _localInstanceTransforms[i]);

        _worldInstanceNormalTransforms.push_back(_worldInstanceTransforms.back().invert().transposed());
    }
}

void ModelNode::setLocalTransform(mat4 localTransform) {
    _localBasicTransform = std::move(localTransform);
    transformUpdate();
}

void ModelNode::render() {
    if (!_enabled)
        return;

    if (isInstancing())
        _mesh->render(instanceCount());
    else
        _mesh->render();
}