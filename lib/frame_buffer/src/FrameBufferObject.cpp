#include "FrameBufferObject.hpp"
#include "GLUtilGeometry.hpp"



FrameBufferObject::FrameBufferObject(ivec2 size, GLuint defaultFbo, Type type) :
_size(size), _defaultFbo(defaultFbo), _fboId(INVALID_GL_ID),
_type(type), _gPosition(INVALID_GL_ID), _gNormal(INVALID_GL_ID), _gAlbedo(INVALID_GL_ID),
_commonTexture(INVALID_GL_ID), _rboDepth(INVALID_GL_ID)
{
    init();
}

FrameBufferObject::~FrameBufferObject() {
    if (_fboId != INVALID_GL_ID)
        glDeleteFramebuffers(1, &_fboId);
    
    if (_gPosition != INVALID_GL_ID)
        glDeleteTextures(1, &_gPosition);
    
    if (_gNormal != INVALID_GL_ID)
        glDeleteTextures(1, &_gNormal);
    
    if (_gAlbedo != INVALID_GL_ID)
        glDeleteTextures(1, &_gAlbedo);
    
    if (_commonTexture != INVALID_GL_ID)
        glDeleteTextures(1, &_commonTexture);
}

ivec2 FrameBufferObject::size() const {
    return _size;
}

void FrameBufferObject::bindWithViewport() {
    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
    glViewport(0, 0, _size.x, _size.y);
}

void FrameBufferObject::init() {
    
    glGenFramebuffers(1, &_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
    
    //TODO: 포멧 효율화
    if (_type == Type::GBuffer) {
        
        // - position color buffer
        glGenTextures(1, &_gPosition);
        glBindTexture(GL_TEXTURE_2D, _gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gPosition, 0);
        GLUtil::GL_ERROR_LOG();
        
        // - normal color buffer
        glGenTextures(1, &_gNormal);
        glBindTexture(GL_TEXTURE_2D, _gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _gNormal, 0);
        GLUtil::GLUtil::GL_ERROR_LOG();
        
        // - color
        glGenTextures(1, &_gAlbedo);
        glBindTexture(GL_TEXTURE_2D, _gAlbedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size.x, _size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _gAlbedo, 0);
        GLUtil::GL_ERROR_LOG();

        
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        GLUtil::GL_ERROR_LOG();
        
        // then also add render buffer object as depth buffer and check for completeness.
        glGenRenderbuffers(1, &_rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, _rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _size.x, _size.y);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rboDepth);
        GLUtil::GL_ERROR_LOG();
        
        
    } else {
        
        glGenTextures(1, &_commonTexture);
        glBindTexture(GL_TEXTURE_2D, _commonTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _commonTexture, 0);
        GLUtil::GL_ERROR_LOG();
        
    }
    
    GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (check != GL_FRAMEBUFFER_COMPLETE) {
        printf("NO COMPLETE... !!!!!!!!!!!!!!!!!!!!!!!!!!! %d \n", (int)check);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFbo);
}
