#include "ModelNode.hpp"
#include "MeshBasic.h"

using namespace std;

ModelNode::ModelNode(Scene *scene, shared_ptr<MeshBasic> mesh, mat4 localTransform)
        : _scene(scene), _mesh(std::move(mesh)), _localTransform(std::move(localTransform)),
          _parent(nullptr) {

}

void ModelNode::addChild(std::shared_ptr<ModelNode> node) {
    _children.emplace_back(node);
    node->_parent = this;
}

void ModelNode::transformUpdate() {
    _worldTransform = _parent ? _parent->worldTransform() * _localTransform : _localTransform;
}

void ModelNode::setLocalTransform(mat4 localTransform) {
    _localTransform = std::move(localTransform);
    transformUpdate();
}

void ModelNode::render() {
    if (_enabled)
        _mesh->render();
}