#ifndef TOYRENDERER_ROOM_HPP
#define TOYRENDERER_ROOM_HPP

#include "MeshBasic.h"
#include "Vector.hpp"

class Room : public MeshBasic {
public:
    Room(float size, vec3 backColor, vec3 topColor, vec3 leftColor, vec3 rightColor, vec3 bottomColor,
         std::string name);

private:
    float _size;
    vec3 _topColor;
    vec3 _backColor;
    vec3 _leftColor;
    vec3 _rightColor;
    vec3 _bottomColor;
};

#endif //TOYRENDERER_ROOM_HPP
