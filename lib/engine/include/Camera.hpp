

#ifndef Camera_hpp
#define Camera_hpp

#include "Matrix.hpp"
#include "Vector.hpp"
#include <iostream>

struct Rect {
    int x;
    int y;
    int w;
    int h;
};

struct Basis3 {
    vec3 viewDir;
    vec3 viewUp;
    vec3 viewSide;
};

/**
 * 관찰 구도를 결정하는 Camera클래스입니다.
 * View, Projection Matrix 등을 만들고, 기타 유틸성 계산기능이 있습니다.
 */
class Camera {

public:
    Camera(vec3 eye, vec3 target, vec3 up=vec3(0, 1, 0));

    vec2 viewportSize() {
        updateMat();
        auto focalLen = (_eye - _target).length();
        auto theta = _fovy * PI / 180.0;
        auto h = tan(theta / 2.0);
        auto vpHeight = 2.0 * h * focalLen;
        auto vpWidth = vpHeight * (static_cast<double>(_screenRect.w)/_screenRect.h);
        return vec2(vpWidth, vpHeight);
    }

    const mat4& viewRotMat() {
        updateMat();
        return _viewRotMat;
    }

    const mat4& viewMat() {
        updateMat();
        return _viewMat;
    }
    
    const mat4& projMat() {
        updateMat();
        return _projMat;
    }
    
    const mat4 viewProjMat() {
        updateMat();
        return _viewProjMat;
    }
    
    void setTarget(vec3 v) {
        _needUpdateMat = true;
        _target = std::move(v);
    }
    
    vec3 target() const {
        return _target;
    }
    
    void setEye(vec3 v) {
        _needUpdateMat = true;
        _eye = std::move(v);
    }
    
    vec3 eye() const {
        return _eye;
    }
    
    void setScreenRect(Rect v) {
        _needUpdateMat = true;
        _screenRect = std::move(v);
    }

    Rect screenRect() const {
        return _screenRect;
    }
    
    ivec2 screenSize() const {
        return ivec2(_screenRect.w, _screenRect.h);
    }
    
    
    void setFovy(float v) {
        _needUpdateMat = true;
        _fovy = std::move(v);
    }
    
    float fovy() const {
        return _fovy;
    }
    
    ivec2 project(vec3 worldPos);
    vec3 unproject(vec3 point) const;
    
    Ray ray(ivec2 screenPos);

    void updateViewPosition(int dir, float delta);

    void updateViewRotation(float yaw, float pitch);
    
    static mat4 createViewMatrix(vec3 target, vec3 eye, vec3 up);


private:
    static Basis3 CreateBasis(vec3 target, vec3 eye, vec3 up);

    mat4 computeViewMat() const;
    mat4 computeProjectionMat() const;
    void updateMat();
    
    vec3 _target;
    vec3 _eye;
    vec3 _up;

    Rect _screenRect;
    float _fovy;

    mat4 _viewRotMat;
    mat4 _viewMat;
    mat4 _projMat;
    mat4 _viewProjMat;
    mat4 _invViewProjectionMat;
    bool _needUpdateMat;
};




#endif /* Camera_hpp */
