

#ifndef Camera_hpp
#define Camera_hpp

#include "Matrix.hpp"
#include "Vector.hpp"

struct Rect {
    Rect(int x_, int y_, int w_, int h_) {
        x = x_;
        y = y_;
        w = w_;
        h = h_;
    }
    int x;
    int y;
    int w;
    int h;
};


/**
 * 관찰 구도를 결정하는 Camera클래스입니다.
 * View, Projection Matrix 등을 만들고, 기타 유틸성 계산기능이 있습니다.
 */
class Camera {
  
public:
    
    Camera();
    
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
    
    void setCameraYAngle(float v) {
        _needUpdateMat = true;
        _cameraYAngle = std::move(v);
    }
    
    float cameraYAngle() const {
        return _cameraYAngle;
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
    
    void setEyeYOffset(float v) {
        _eyeYOffet = v;
        _needUpdateMat = true;
    }
    
    ivec2 project(vec3 worldPos);
    vec3 unproject(vec3 point) const;
    
    Ray ray(ivec2 screenPos);
    
    static mat4 createViewMatrix(vec3 target, vec3 eye, vec3 up);
    
private:
    struct Basis3 {
        vec3 vDir;
        vec3 vUp;
        vec3 vSide;
    };
    
    mat4 computeViewMat() const;
    mat4 computeProjectionMat() const;
    
    void updateMat();
    
    vec3 _target;
    vec3 _eye;
    float _cameraYAngle; //up (0,1,0)
    float _eyeYOffet;
    
    Rect _screenRect;
    float _fovy;
    
    mat4 _viewMat;
    mat4 _projMat;
    mat4 _viewProjMat;
    mat4 _invViewProjectionMat;
    
    bool _needUpdateMat;
    
};




#endif /* Camera_hpp */
