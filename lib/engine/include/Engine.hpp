#ifndef RenderEngine_hpp
#define RenderEngine_hpp

#include <memory>
#include "Vector.hpp"
#include "Matrix.hpp"
#include <mutex>

class IBLPreprocessor;
class Scene;
class ShaderManager;

enum class SceneType : int {
    SampleScene0 = 0,
    SampleScene1 = 1
};

class RenderEngine {
    friend class PBRScene;
    friend class RayTraceScene;
    friend class IBLPreprocessor;

public:
    RenderEngine(SceneType sceneType);
    ~RenderEngine();

    void prepare();

    void render();

    void initScene();

    void setScreenSize(int w, int h);

    //dir: forward:0, backward:1, left:2, right:3
    void updateViewPosition(int dir, float delta);

    void updateViewRotation(float yaw, float pitch);

private:
    uint _defaultFBO;

    bool _init = false;

    SceneType _sceneType = SceneType::SampleScene0;

    ivec2 _screenSize;

    std::mutex _sceneMutex;

    Scene* _scene = nullptr;

    std::shared_ptr<ShaderManager> _shaderManager;
};

#endif /* RenderEngine_hpp */
