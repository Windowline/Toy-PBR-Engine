

#ifndef GBufferShader_hpp
#define GBufferShader_hpp

#include "BasicLightingShader.hpp"

/**
 * Deferred Render시 라이팅/그림자 정보를 위한 텍스처를 MRT방식으로 그리는 셰이더입니다.
 * position, normal, albedo를 한번에 그립니다.
 */
class GBufferShader : public BasicLightingShader {

public:
    GBufferShader();
    virtual bool load() override;
    virtual void useProgram() override;
    
};


#endif /* GBufferShader_hpp */
