#ifndef ShaderBase_hpp
#define ShaderBase_hpp
#include <cassert>
//#include <OpenGLES/ES3/gl.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <array>
#include <string>

#define INVALID_GL_ID 0
#define INVALID_GL_LOCATION -1
#define Shader_Invalid_Location -1

const int VertexAttribute_MAX = 15;

/*
 * 셰이더 기본 클래스입니다.
 * 기본적인 어트리뷰트를 적용합니다.
 */
class ShaderBase {
   public:
    
    ShaderBase() {
        _programID = INVALID_GL_ID;
        _enabledAttributes.fill(false);
        _positionAttribLocation = Shader_Invalid_Location;
        _colorAttribLocation = Shader_Invalid_Location;
        _normalAttribLocation = Shader_Invalid_Location;
    }
    
    virtual ~ShaderBase() {
        unload();
    }
    
    virtual bool load() = 0;
    
    bool unload() {
        if (_programID) {
            glDeleteProgram (_programID);
        }
        
        _programID = INVALID_GL_ID;
        return true;
    }

    GLint getProgramID() const {
        return _programID;
    }
    
    virtual void useProgram() {
        glUseProgram(_programID);
    }
    
    void checkGLError() {
        GLenum ret = glGetError();
        if(ret != GL_NO_ERROR) {
            printf(">> GL_ERROR: 0x%04x \n", ret);
        }
    }
    
    GLuint loadProgram(const char *vertShaderSrc, const char *fragShaderSrc) {
        GLuint vertexShader;
        GLuint fragmentShader;
        GLuint programObject;
        GLint linked;

        std::string vsCode, fsCode;


        vsCode = vertShaderSrc;
        fsCode = fragShaderSrc;
        
        // Load the vertex/fragment shaders
        vertexShader = loadShader(GL_VERTEX_SHADER, vsCode.c_str());
        if (vertexShader == 0) {
            return 0;
        }

        fragmentShader = loadShader(GL_FRAGMENT_SHADER, fsCode.c_str());
        if (fragmentShader == 0) {
            glDeleteShader(vertexShader);
            return 0;
        }

        // Create the program object
        programObject = glCreateProgram();

        if (programObject == 0) {
            return 0;
        }
        
        glAttachShader ( programObject, vertexShader );
        glAttachShader ( programObject, fragmentShader );
        
        // Link the program
        glLinkProgram ( programObject );
        
        // Check the link status
        glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );
        
        if (!linked)
        {
            GLint infoLen = 0;
            
            glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
            
            if ( infoLen > 1 )
            {
    //#ifdef __NOT_SHADER_LOG__
                
                char* infoLog = (char*)malloc (sizeof(char) * infoLen );
                
                glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
                printf ( "Error linking program:\n%s\n", infoLog );
                
                free ( infoLog );
    //#endif
            }
            
            glDeleteProgram ( programObject );
            return 0;
        }
        
        // Free up no longer needed shader resources
        glDeleteShader ( vertexShader );
        glDeleteShader ( fragmentShader );
        
        return programObject;
    }
    
    GLuint loadShader(GLenum type, const char *shaderSrc) {
        
        GLuint shader;
        GLint compiled;
        GLint infoLen = 0;
        
        // Create the shader object
        shader = glCreateShader ( type );
        
        if ( shader == 0 )
            return 0;

        // Load the shader source
        glShaderSource ( shader, 1, &shaderSrc, NULL );
        
        // Compile the shader
        glCompileShader ( shader );
        
        // Check the compile status
        glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );
        
        
        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
        
        if ( infoLen > 1 ){
            char* infoLog = (char*)malloc(sizeof(char) * infoLen );
            
            glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
            printf( "compiling log shader[%s:%d]:\n%s\n", _name.c_str(), type, infoLog );
            
            free(infoLog);
        }
        
        if ( !compiled ) {
            glDeleteShader ( shader );
            return 0;
        }
        return shader;
    }
    
    std::array<bool, VertexAttribute_MAX> const &getEnabledAttributes() const {
        return _enabledAttributes;
    }

    void vertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                             const GLvoid *ptr) {
        glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
    }

    bool isLoaded() const {
        return _programID != INVALID_GL_ID;
    }
    
    /* Position */
    void positionAttribLocation() {
        _positionAttribLocation = glGetAttribLocation(_programID, "a_position");
        assert(_positionAttribLocation != Shader_Invalid_Location);
    }
    
    void positionAttribPointer(const GLvoid *ptr, int nChannels) {
        if (!_enabledAttributes[_positionAttribLocation]) {
            _enabledAttributes[_positionAttribLocation] = true;
            glEnableVertexAttribArray(_positionAttribLocation);
        }
        vertexAttribPointer(_positionAttribLocation, nChannels, GL_FLOAT, GL_FALSE, 0, ptr);
    }
    
    
    /* Color */
    void colorAttribLocation() {
        _colorAttribLocation = glGetAttribLocation(_programID, "a_color");
        assert(_colorAttribLocation != Shader_Invalid_Location);
    }

    void colorAttribPointer(const GLvoid *ptr, int nChannels) {
        if (!_enabledAttributes[_colorAttribLocation]) {
            _enabledAttributes[_colorAttribLocation] = true;
            glEnableVertexAttribArray(_colorAttribLocation);
        }
        vertexAttribPointer(_colorAttribLocation, nChannels, GL_FLOAT, GL_FALSE, 0, ptr);
    }

    
    
    /* Normal */
    // normal attrib location
    void normalAttribLocation() {
        _normalAttribLocation = glGetAttribLocation(_programID, "a_normal");
        assert(_normalAttribLocation != Shader_Invalid_Location);
    }

    // normal attrib func
    void normalAttribPointer(const GLvoid *ptr, int nChannels) {
        if (!_enabledAttributes[_normalAttribLocation]) {
            _enabledAttributes[_normalAttribLocation] = true;
            glEnableVertexAttribArray(_normalAttribLocation);
        }
        vertexAttribPointer(_normalAttribLocation, nChannels, GL_FLOAT, GL_FALSE, 0, ptr);
    }

    
protected:
    //attrib
    GLint _positionAttribLocation;
    GLint _colorAttribLocation;
    GLint _normalAttribLocation;
    
    GLuint _programID;
    std::string _name;
    std::array<bool, VertexAttribute_MAX> _enabledAttributes;
};

#endif /* ShaderBase_hpp */
