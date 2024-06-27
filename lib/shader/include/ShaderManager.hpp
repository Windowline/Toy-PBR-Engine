

#ifndef ShaderManager_hpp
#define ShaderManager_hpp

#include <memory>
#include <vector>
#include <unordered_map>

class BasicShader;

enum eShaderProgram : int
{
    eShaderProgram_Basic = 1,
    eShaderProgram_GBuffer = 2,
    eShaderProgram_TexturePass = 3,
    eShaderProgram_ShadowRender = 4,
    eShaderProgram_ShadowDepth = 5,
    eShaderProgram_SSAO = 6,
    eShaderProgram_SSAO_BLUR = 7,
    eShaderProgram_EquirectangularToCubemap = 8,
    eShaderProgram_Irradiance = 9,
    eShaderProgram_Prefilter = 10,
    eShaderProgram_BRDF = 11,
    eShaderProgram_BG = 12,
    eShaderProgram_PBR = 13,
    eShaderProgram_DeferredPBR = 14,
    eShaderProgram_Last = 15
};


/**
 * 모든 셰이더들을 관리하는 클래스입니다.
 */
class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    void addShader(int shaderId, std::shared_ptr<BasicShader> const& shaderProgram);
    void removeShader(int shaderId);
    
    template<typename T>
    std::shared_ptr<T> getShader(int shaderId) const {
        return std::static_pointer_cast<T>(findShader(shaderId));
    }
    
    std::shared_ptr<BasicShader> findShader(int shaderId) const;

    std::shared_ptr<BasicShader> getActiveShader() const;
    
    void setActiveShader(std::shared_ptr<BasicShader> const & shader);
        
    template<typename T>
    std::shared_ptr<T> setActiveShader(int shaderId) {
        setActiveShader(getShader<T>(shaderId));
        return getShader<T>(shaderId);
    }
    
    void loadAllPrograms();
    void unloadAllPrograms();

    void checkGLError();
private:
    std::unordered_map<int, std::shared_ptr<BasicShader>> _shaderMap;
    std::shared_ptr<BasicShader> _activeShader;
};




#endif /* ShaderManager_hpp */
