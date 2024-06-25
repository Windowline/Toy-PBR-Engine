#include "Camera.hpp"
#include <cassert>

Camera::Camera(vec3 eye, vec3 target, vec3 up) :
                          _eye(std::move(eye)),
                          _target(std::move(target)),
                          _up(std::move(up)),
                          _fovy(45.f),
                          _needUpdateMat(false),
                          _screenRect({0, 0, 0, 0})
{}


Ray Camera::ray(ivec2 screenPos) {
    
    screenPos.y = _screenRect.h - screenPos.y;
    
    vec3 near = unproject(vec3(screenPos.x, screenPos.y, 0.f));
    vec3 far = unproject(vec3(screenPos.x, screenPos.y, 1.f));
    return Ray(near, (far - near).normalized());
}

ivec2 Camera::project(vec3 worldPos) {
    
    vec4 ret = viewProjMat().multiplication1n4(vec4(worldPos, 1.0));
    ret.x /= ret.w;
    ret.y /= ret.w;
    ret.z /= ret.w;
    
    ret.x = ret.x * 0.5 + 0.5;
    ret.y = ret.y * 0.5 + 0.5;
    ret.z = ret.z * 0.5 + 0.5;
    
    ret.x = ret.x * _screenRect.w + _screenRect.x;
    ret.y = _screenRect.h - (ret.y * _screenRect.h + _screenRect.y);
    
    return ivec2(ret.x, ret.y);
}

vec3 Camera::unproject(vec3 point) const {
    vec4 input(point, 1.0);
    input.x = (input.x - _screenRect.x) / _screenRect.w;
    input.y = (input.y - _screenRect.y) / _screenRect.h;
    input.x = input.x * 2.0 - 1.0;
    input.y = input.y * 2.0 - 1.0;
    input.z = input.z * 2.0 - 1.0;
    vec4 world = _invViewProjectionMat.multiplication1n4(input);

    world.x /= world.w;
    world.y /= world.w;
    world.z /= world.w;

    return world.xyz();
}

Basis3 Camera::CreateBasis(vec3 target, vec3 eye, vec3 up) {
    //카메라 관점 정규직교 좌표계 생성
    vec3 viewDir = (target - eye).normalized();
    vec3 viewUp = (up - (viewDir * up.dot(viewDir))).normalized();
    vec3 viewSide = viewDir.cross(viewUp).normalized();
    return {viewDir, viewUp, viewSide};
}

void Camera::updateViewPosition(int dir, float delta) {
    const auto& basis = Camera::CreateBasis(_target, _eye, _up);
    vec3 updatedEye = _eye;
    vec3 updateTarget = _target;

    if (dir == 0)
        updatedEye += basis.viewDir * delta;
    else if (dir == 1)
        updatedEye -= basis.viewDir * delta;
    else if (dir == 2) {
        updatedEye -= basis.viewSide * delta;
        updateTarget -= basis.viewSide * delta;
    } else if (dir == 3) {
        updatedEye += basis.viewSide * delta;
        updateTarget += basis.viewSide * delta;
    } else {
        std::cout << "unexpected dir " << std:: endl;
    }

    _eye = updatedEye;
    _target = updateTarget;
    _needUpdateMat = true;
}

void Camera::updateViewRotation(float yaw, float pitch) {
    auto d2r = [](float d)->float {return d * 3.141592653589793238462 / 180.0f;};

    vec3 viewDir;
    viewDir.x = cos(d2r(yaw)) * cos(d2r(pitch));
    viewDir.y = sin(d2r(pitch));
    viewDir.z = sin(d2r(yaw)) * cos(d2r(pitch));
    viewDir.normalize();

    float distance = (_eye - _target).length();
    _target = viewDir * distance;

    _needUpdateMat = true;
}


mat4 Camera::createViewMatrix(vec3 target, vec3 eye, vec3 up) {
    const auto& basis = Camera::CreateBasis(target, eye, up);
    
    mat4 ret;
    ret.x = vec4(std::move(basis.viewSide), 0);
    ret.y = vec4(std::move(basis.viewUp), 0);
    ret.z = vec4(std::move(-basis.viewDir), 0);
    ret.w = vec4(0, 0, 0, 1);

    vec4 eyeInverse = ret * vec4(-eye, 1);
    ret = ret.transposed();
    ret.w = std::move(eyeInverse);
    
    return ret;
}

mat4 Camera::computeViewMat() const {
    return Camera::createViewMatrix(_target,
                                    _eye,
                                    _up);
}


mat4 Camera::computeProjectionMat() const {
    
    return mat4::Frustum(static_cast<float>(_screenRect.w),
                         static_cast<float>(_screenRect.h),
                         static_cast<float>(_fovy),
                         0.01f,
                         10000.f);
}


void Camera::updateMat() {
    if (!_needUpdateMat) {
        return;
    }

    assert(_screenRect.w > 0 && _screenRect.h > 0);

    _viewMat = computeViewMat();

    _viewRotMat = _viewMat;
    _viewProjMat.w.x = 0.0;
    _viewProjMat.w.y = 0.0;
    _viewProjMat.w.z = 0.0;
    _viewProjMat.w.w = 1.0;

    _projMat = computeProjectionMat();
    _viewProjMat = _viewMat * _projMat;
    _invViewProjectionMat = _viewProjMat.invert();
    
    _needUpdateMat = false;
}



