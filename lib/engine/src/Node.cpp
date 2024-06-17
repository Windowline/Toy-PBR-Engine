#include "Scene.hpp"
#include "Node.hpp"
#include "Mesh.hpp"
#include "Node.hpp"
#include "Camera.hpp"
#include "ShadowDepthShader.hpp"
#include "BasicLightingShader.hpp"
#include "GBufferShader.hpp"
#include "AlbedoColorShader.hpp"

Node::Node(Scene* scene, std::shared_ptr<Mesh> mesh, mat4 localTransform)
        : _scene(scene), _mesh(std::move(mesh)), _localTransform(std::move(localTransform)),
          _parent(nullptr)
{

}

Node::~Node() {
    _children.clear();
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

    std::shared_ptr<ShaderBase> activeShader = _scene->shaderManager()->getActiveShader();

    switch (_shaderType) {
        case eShaderProgram_ShadowDepth:
            setShadowDepthShader(std::static_pointer_cast<ShadowDepthShader>(activeShader)); break;

        case eShaderProgram_GBuffer:
            setGBufferShader(std::static_pointer_cast<GBufferShader>(activeShader)); break;

        case eShaderProgram_Default:
            setBasicPhongShader(std::static_pointer_cast<BasicLightingShader>(activeShader)); break;

        case eShaderProgram_ALBEDO_COLOR: {
            mat4 mvp = _worldTransform * _scene->camera()->viewMat() * _scene->camera()->projMat();
            std::static_pointer_cast<AlbedoColorShader>(activeShader)->mvpUniformMatrix4fv(mvp.pointer());
            break;
        }

        default:
            printf("?????\n");
            break;
    }

    _mesh->render(activeShader);
}


//private method - shader set

void Node::setShadowDepthShader(std::shared_ptr<ShadowDepthShader> shader) {

    const mat4& view = _scene->camera()->viewMat();
    const mat4& proj = _scene->camera()->projMat();
    mat4 wordlView = _worldTransform * view;
    mat4 shadowMVP = _worldTransform * _scene->shadowLightViewProjection();

    shader->projMatUniformMatrix4fv(proj.pointer());
    shader->worldViewMatUniformMatrix4fv(wordlView.pointer());
    shader->shadowMVPUniformMatrix4fv(shadowMVP.pointer());
}


void Node::setGBufferShader(std::shared_ptr<GBufferShader> shader) {

    //light env
    setSceneLightCommonProperty(std::static_pointer_cast<BasicLightingShader>(shader));

    const mat4& view = _scene->camera()->viewMat();
    const mat4& proj = _scene->camera()->projMat();
    mat4 wordlView = _worldTransform * view;

    shader->projMatUniformMatrix4fv(proj.pointer());
    shader->worldMatUniformMatrix4fv(_worldTransform.pointer());
    shader->worldViewMatUniformMatrix4fv(wordlView.pointer());
    shader->worldNormalMatUniformMatrix4fv(_worldTransform.invert().transposed().pointer());
}


void Node::setBasicPhongShader(std::shared_ptr<BasicLightingShader> shader) {

    //light env
    setSceneLightCommonProperty(shader);


    const mat4& view = _scene->camera()->viewMat();
    const mat4& proj = _scene->camera()->projMat();
    mat4 wordlView = _worldTransform * view;

    shader->projMatUniformMatrix4fv(proj.pointer());
    shader->worldMatUniformMatrix4fv(_worldTransform.pointer());
    shader->worldViewMatUniformMatrix4fv(wordlView.pointer());
    shader->worldNormalMatUniformMatrix4fv(_worldTransform.invert().transposed().pointer());


}

void Node::setSceneLightCommonProperty(std::shared_ptr<BasicLightingShader>const & shader) {

    auto& ambientColor = _scene->ambientColor();
    auto& diffuseColor = _scene->diffuseColor();
    auto& specularColor = _scene->specularColor();


    shader->ambientColorUniform3f(ambientColor.x, ambientColor.y, ambientColor.z);
    shader->diffuseColorUniform3f(diffuseColor.x, diffuseColor.y, diffuseColor.z);
    shader->specularColorUniform3f(specularColor.x, specularColor.y, specularColor.z);


    const int LIGHT_COMPONENT_COUNT = 3;
    float* lightPosArray = new float[_scene->lightPositions().size() * LIGHT_COMPONENT_COUNT];
    int idx = 0;
    for (auto& lightPos : _scene->lightPositions()) {
        lightPosArray[idx++] = lightPos.x;
        lightPosArray[idx++] = lightPos.y;
        lightPosArray[idx++] = lightPos.z;
    }
    shader->worldLightPosUniform3fv(lightPosArray, (int)_scene->lightPositions().size());
    delete [] lightPosArray;


    const auto cameraEye = _scene->camera()->eye();
    shader->worldEyePositionUniform3f(cameraEye.x, cameraEye.y, cameraEye.z);
}

//TODO
bool Node::hitTest(Ray ray, vec3& outPos) {
    bool ret = false;

    if (_mesh->rayCheckType() == Mesh::RayCheckType::Sphere) {
        vec3 origin = _worldTransform.multiplication1n4(vec4(0, 0, 0, 1)).xyz();
        ret = ray.intersectWithSphere(origin, _mesh->boundingSphereRadius(), outPos);

    } else if (_mesh->rayCheckType() == Mesh::RayCheckType::Box) {
        ret = ray.intersectWithBox(_mesh->boundingBox().minPos, _mesh->boundingBox().maxPos, outPos);

    } else if (_mesh->rayCheckType() == Mesh::RayCheckType::Plane) {

    }

    return true;
}

//TODO
vec3 Node::worldNormalAt(vec3 point) {

    if (_mesh->rayCheckType() == Mesh::RayCheckType::Sphere) {
        vec3 worldOrigin = _worldTransform.multiplication1n4(vec4(0, 0, 0, 1)).xyz();
        return (point - worldOrigin).normalized();

    } else if (_mesh->rayCheckType() == Mesh::RayCheckType::Box) {

    } else if (_mesh->rayCheckType() == Mesh::RayCheckType::Plane) {

    }

    return vec3(0, 0, 0);
}
