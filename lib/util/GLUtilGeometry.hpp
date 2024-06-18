
#ifndef GLUtil_h
#define GLUtil_h

#include "Vector.hpp"
#include <stdio.h>
#include <glad/glad.h>

#define INVALID_GL_ID 0

class GLUtil {

public:
    static void GL_ERROR_LOG() {
        int error_code = glGetError();
        if(error_code!=GL_NO_ERROR) {
            printf("CS== gl err: %d ==\n", error_code);
        }
    }
};


namespace GLUtilGeometry {
    static vec2 VERT_QUAD[4] = {
            vec2(-1.f, -1.f),
            vec2(-1.f, 1.f),
            vec2(1.f, -1.f),
            vec2(1.f, 1.f),
    };

    static vec2 TEXCOORD_QUAD[4] = {
            vec2(0, 0),
            vec2(0, 1),
            vec2(1, 0),
            vec2(1, 1)
    };

}

#endif /* GLUtil_h */
