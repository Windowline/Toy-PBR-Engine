#ifndef TOYRENDERER_ROOM_HPP
#define TOYRENDERER_ROOM_HPP

#include "MeshBasic.h"
#include "Vector.hpp"

class Room : public MeshBasic {
public:
    Room(float size, vec3 backColor, vec3 topColor, vec3 leftColor, vec3 rightColor, vec3 bottomColor,
         std::string name);

    const vector<vec3>& trianglePostions() const {
        return _trianglePositions;
    }

    const vector<vec3>& triangleNormals() const {
        return _triangleNormals;
    }

    vector<vec3> faceColors() const {
        static vector<vec3> ret = {
                _backColor, _topColor, _leftColor, _rightColor, _bottomColor
        };
        return ret;
    }

private:
    float _size;
    vec3 _topColor;
    vec3 _backColor;
    vec3 _leftColor;
    vec3 _rightColor;
    vec3 _bottomColor;

    vector<vec3> _trianglePositions;
    vector<vec3> _triangleNormals;
};

#endif //TOYRENDERER_ROOM_HPP
