#include <memory>
//#include <OpenGLES/ES3/gl.h>
#include <glad/glad.h>
#include "Vector.hpp"

/*
 * off screen에 결과물을 그려두기 위한 FrameBufferObject입니다.
 */
class FrameBufferObject {
public:
    
    enum class Type : int {
        GBuffer = 0,
        Common = 1,
        Depth = 2,
    };
    
    
    FrameBufferObject(ivec2 size, GLuint defaultFbo, Type type);
    ~FrameBufferObject();

    ivec2 size() const;
    void bindWithViewport();
    GLuint texture() const;
    
    GLuint gNormalTexture() const {
        return _gNormal;
    }
    
    GLuint gAlbedoTexture() const {
        return _gAlbedo;
    }

    GLuint gViewNormalTexture() const {
        return _gViewNormal;
    }

    GLuint commonTexture() const {
        return _commonTexture;
    }

private:
    void init();

private:
    Type _type;
    unsigned int _defaultFbo;
    ivec2 _size;
    unsigned int _fboId;

    unsigned int _gNormal;
    unsigned int _gViewNormal;
    unsigned int _gAlbedo;
    unsigned int _commonTexture;

    unsigned int _rboDepth;
};

