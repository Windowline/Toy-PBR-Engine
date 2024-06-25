#ifndef TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP
#define TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP

class ShaderManager;
class FullQuad;
class Cube;

#include "Camera.hpp"
#include <memory>
#include <string_view>

using namespace std;

class IBLPreprocessor {
public:
    IBLPreprocessor(shared_ptr<ShaderManager> shaderManager, string_view path);

    void build();

    ~IBLPreprocessor();

    unsigned int hdrTexture() const {
        return _hdrTexture;
    }

    unsigned int irradianceCubeMap() const {
        return _irradianceCubeMap;
    }

    unsigned int prefilterCubeMap() const {
        return _prefilterCubeMap;
    }

    unsigned int envCubemap() const {
        return _envCubemap;
    }

    unsigned int brdfLUTTexture() const {
        return _brdfLUTTexture;
    }


private:
    void renderEnvironmentCubeMapFromHDR(unsigned int captureFBO, const array<mat4, 6>& captureViews, const mat4& captureProj);
    void renderIrradianceCubeMap(unsigned int captureFBO, unsigned int captureRBO, const array<mat4, 6>& captureViews, const mat4& captureProj);
    void renderPrefilterCubemap(unsigned int captureFBO, unsigned int captureRBO, const array<mat4, 6>& captureViews, const mat4& captureProj);
    void renderBRDFLUT(unsigned int captureFBO, unsigned int captureRBO);

    shared_ptr<ShaderManager> _shaderManager;
    string_view _path;
    unique_ptr<FullQuad> _fullQuad;
    unique_ptr<Cube> _fullCube;

    unsigned int _hdrTexture = 0;
    unsigned int _irradianceCubeMap = 0;
    unsigned int _prefilterCubeMap = 0;
    unsigned int _envCubemap = 0;
    unsigned int _brdfLUTTexture = 0;

    unsigned int _captureFBO = 0;
    unsigned int _captureRBO = 0;

};

#endif //TOYRENDERER_HDRTOCUBEMAPCONVERTER_HPP
