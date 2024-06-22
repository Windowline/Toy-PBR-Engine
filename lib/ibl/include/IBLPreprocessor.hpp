#ifndef TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP
#define TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP

class ShaderManager;
class FullQuad;

#include <memory>
#include <string_view>

class IBLPreprocessor {
public:
    IBLPreprocessor(std::shared_ptr<ShaderManager> shaderManager, std::string_view path);

    void build();

    ~IBLPreprocessor();

    unsigned int hdrTexture() const {
        return _hdrTexture;
    }

    unsigned int irradianceMap() const {
        return _irradianceMap;
    }

    unsigned int prefilterMap() const {
        return _prefilterMap;
    }

    unsigned int envCubemap() const {
        return _envCubemap;
    }

    unsigned int brdfLUTTexture() const {
        return _brdfLUTTexture;
    }


private:
    std::shared_ptr<ShaderManager> _shaderManager;
    std::string_view _path;
    std::unique_ptr<FullQuad> _fullQuad;

    unsigned int _hdrTexture = 0;
    unsigned int _irradianceMap = 0;
    unsigned int _prefilterMap = 0;
    unsigned int _envCubemap = 0;
    unsigned int _brdfLUTTexture = 0;

    unsigned int _captureFBO = 0;
    unsigned int _captureRBO = 0;

};

#endif //TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP
