#ifndef TOYRENDERER_UXUTIL_HPP
#define TOYRENDERER_UXUTIL_HPP

#include "Engine.hpp"

namespace UXUtil {
    enum class ViewSpaceMoveDirection : int {
        FORWARD = 0,
        BACKWARD = 1,
        LEFT = 2,
        RIGHT = 3,
        NOTHING = 4
    };

    constexpr float ROTATION_SENSITIVITY = 0.1f;
    constexpr float MOVE_SPEED = 32.f;

    float curYaw = -90.0;
    float curPitch = 0.f;
    bool isFirstRotation = true;
    float viewLastX = 500.f;
    float viewLastY = 500.f;

    void setInitialMousePosition(float x, float y) {
        viewLastX = x;
        viewLastY = y;
    }

    void updateViewRotation(float x, float y, RenderEngine *engine) {
        if (isFirstRotation) {
            viewLastX = x;
            viewLastY = y;
            isFirstRotation = false;
        }

        float xOffset = x - viewLastX;
        float yOffset = viewLastY - y;

        viewLastX = x;
        viewLastY = y;

        xOffset *= ROTATION_SENSITIVITY;
        yOffset *= ROTATION_SENSITIVITY;

        curYaw += xOffset;
        curPitch += yOffset;

        if (curPitch > 89.0f)
            curPitch = 89.0f;
        if (curPitch < -89.0f)
            curPitch = -89.0f;

        engine->updateViewRotation(curYaw, curPitch);
    }

    void updateViewPosition(ViewSpaceMoveDirection dir, float deltaTime, RenderEngine* engine) {
        if (dir == ViewSpaceMoveDirection::NOTHING)
            return;
        engine->updateViewPosition((int)dir, MOVE_SPEED * deltaTime);
    }

};

#endif //TOYRENDERER_UXUTIL_HPP
