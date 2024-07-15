#include "Node.hpp"
#include "MeshBasic.h"

using namespace std;

Node::Node(PBRScene *scene, shared_ptr<MeshBasic> mesh, mat4 localTransform)
        : _scene(scene), _mesh(std::move(mesh)), _localTransform(std::move(localTransform)),
          _parent(nullptr) {

}

void Node::addChild(std::shared_ptr<Node> node) {
    _children.emplace_back(node);
    node->_parent = this;
}

void Node::transformUpdate() {
    _worldTransform = _parent ? _parent->worldTransform() * _localTransform : _localTransform;
}

void Node::setLocalTransform(mat4 localTransform) {
    _localTransform = std::move(localTransform);
    transformUpdate();
}

void Node::render() {
    if (_enabled)
        _mesh->render();
}