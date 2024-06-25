
#ifndef Scene_hpp
#define Scene_hpp
#include <functional>
#include "Vector.hpp"
#include "Matrix.hpp"

class RenderEngine;
class Camera;
class Node;
class ShaderManager;
class FrameBufferObject;
class FullQuad;
class TexturePassShader;
class IBLPreprocessor;
class Cube;

/**
 * 장면에 필요한 렌더링 오브젝트, 빛, 그림자 정보 등을 관리하고,
 * 실질적으로 오브젝트와 셰이더, 프레임버퍼 등을 이용하여 장면을 그리는 클래스입니다.
 */
class Scene {

public:

    Scene(RenderEngine* engine, unsigned int defaultFBO);

    virtual ~Scene();

    void setScreenSize(int w, int h);

    void updateViewPosition(int dir, float delta);

    void updateViewRotation(float yaw, float pitch);

    void visitNodes(std::shared_ptr<Node> node, std::function<void(std::shared_ptr<Node>)> func);

    void render();

    std::shared_ptr<ShaderManager> shaderManager();

    std::shared_ptr<Camera> camera();

    std::shared_ptr<FrameBufferObject> gBuffer() const {
        return _gBuffer;
    }

    const mat4& shadowLightViewProjection() const {
        return _shadowLightViewProjection;
    }

    const vec3& ambientColor() const {
        return _ambientColor;
    }

    const vec3& diffuseColor() const {
        return _diffuseColor;
    }

    const vec3& specularColor() const {
        return _specularColor;
    }

    std::vector<vec3> lightPositions() const {
        return _lightPositions;
    }

private:
    void buildSSAOInfo();

    void renderSkyBox();

    void renderPBR();

    void debugIBL(); //for debug
    void renderQuad(unsigned int texture, ivec2 screenSize); //for debug

    RenderEngine* _engine;
    unsigned int _defaultFBO;
    bool _rootTransformDirty;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Node> _rootNode;
    std::shared_ptr<Node> _room;
    std::shared_ptr<Node> _cube;
    std::shared_ptr<Node> _sphere;
    std::shared_ptr<Node> _lightSphere;//조명에 위치한 흰색 구
    std::shared_ptr<Node> _model;
    std::shared_ptr<FrameBufferObject> _gBuffer;
    std::shared_ptr<FrameBufferObject> _shadowDepthBuffer;

    mat4 _shadowLightView;
    mat4 _shadowLightProj;
    mat4 _shadowLightViewProjection;

    vec3 _ambientColor;
    vec3 _diffuseColor;
    vec3 _specularColor;
    std::vector<vec3> _lightPositions;

    //ssao
    std::vector<vec3> _ssaoKernel;
    float _ssaoNoise[16*3];
    unsigned int _ssaoNoiseTexture;
    std::shared_ptr<FrameBufferObject> _ssaoFBO;
    std::shared_ptr<FrameBufferObject> _ssaoBlurFBO;

    std::unique_ptr<FullQuad> _fullQuad;
    std::unique_ptr<Cube> _fullCube;
    std::unique_ptr<TexturePassShader> _textureShader;
    std::unique_ptr<IBLPreprocessor> _iblPreprocessor;

};

#endif /* Scene_hpp */
