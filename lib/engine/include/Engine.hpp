#ifndef RenderEngine_hpp
#define RenderEngine_hpp

#include <memory>
#include "Vector.hpp"
#include "Matrix.hpp"
#include <mutex>

class IBLPreprocessor;
class Scene;
class RayTraceScene;
class ShaderManager;

class RenderEngine {

    friend class Scene;
    friend class RayTraceScene;
    friend class IBLPreprocessor;

public:
    RenderEngine();

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

    std::shared_ptr<ShaderManager> _shaderManager;

//    std::shared_ptr<Scene> _scene;
    std::shared_ptr<RayTraceScene> _scene;

    ivec2 _screenSize;

    std::mutex _sceneMutex;
};

#endif /* RenderEngine_hpp */
