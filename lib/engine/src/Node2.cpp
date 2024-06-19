#include "Node2.hpp"
#include "BasicMeshInterface.h"

using namespace std;

Node2::Node2(Scene *scene, shared_ptr<BasicMeshInterface> mesh, mat4 localTransform)
        : _scene(scene), _mesh(std::move(mesh)), _localTransform(std::move(localTransform)),
          _parent(nullptr) {

}

void Node2::addChild(std::shared_ptr<Node2> node) {
    _children.emplace_back(node);
    node->_parent = this;
}

void Node2::transformUpdate() {
    _worldTransform = _parent ? _parent->worldTransform() * _localTransform : _localTransform;
}

void Node2::setLocalTransform(mat4 localTransform) {
    _localTransform = std::move(localTransform);
    transformUpdate();
}

void Node2::render() {
    _mesh->render();
}