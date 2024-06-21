

#ifndef Camera_hpp
#define Camera_hpp

#include "Matrix.hpp"
#include "Vector.hpp"
#include <iostream>

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
    
    void setScreenRect(Rect v) {
        _needUpdateMat = true;
        _screenRect = std::move(v);
    }

    void updateViewPosition(int dir, float delta) {
        auto createBasis = [](vec3 eye, vec3 up, vec3 targetPos) -> Basis3 {
            vec3 vDir = (targetPos - eye).normalized();
            vec3 vUp = (up - (vDir * up.dot(vDir))).normalized();
            vec3 vSide = vDir.cross(vUp);
            return Basis3{ vDir, vUp, vSide };
        };

        const auto& basis = createBasis(_eye, vec3(0, 1, 0), _target);

        vec3 updatedEye = _eye;

        if (dir == 0)
            updatedEye += basis.vDir * delta;
        else if (dir == 1)
            updatedEye -= basis.vDir * delta;
        else if (dir == 2)
            updatedEye -= basis.vSide * delta;
        else if (dir == 3)
            updatedEye += basis.vSide * delta;
        else
            std::cout << "unexpected dir " << std:: endl;

        _eye = updatedEye;
        _needUpdateMat = true;
    }

    void updateViewRotation(float yaw, float pitch) {
        printf("yaw:  %f \n", yaw);

        auto d2r = [](float d)->float {return d * 3.141592653589793238462 / 180.0f;};

        vec3 front;
        front.x = cos(d2r(yaw)) * cos(d2r(pitch));
        front.y = sin(d2r(pitch));
        front.z = sin(d2r(yaw)) * cos(d2r(pitch));
        front.normalize();

        float distance = (_eye - _target).length();
        _target = front * distance;

        _needUpdateMat = true;
    }

    void updateView(vec3 target, vec3 eye, vec3 up) {
//        _needUpdateMat = true;
//        _target = std::move(target);
//        _eye = std::move(eye);
//        _up = std::move(up);
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
    
    Rect _screenRect;
    float _fovy;
    
    mat4 _viewMat;
    mat4 _projMat;
    mat4 _viewProjMat;
    mat4 _invViewProjectionMat;
    
    bool _needUpdateMat;
    
};




#endif /* Camera_hpp */
