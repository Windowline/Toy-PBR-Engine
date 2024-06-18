#include "TestShaderTmp.hpp"

const char *vertexShaderSource2 = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;
        uniform mat4 u_mvp;
        out vec3 v_color;
        out vec3 v_normal;

        void main()
        {
            v_color = a_color;
            gl_Position = u_mvp * vec4(a_position.x, a_position.y, a_position.z, 1.0);
        }
);

const char *fragmentShaderSource2 = R(
        in vec3 v_color;
        out vec4 FragColor;
        void main()
        {
            FragColor = vec4(v_color.x, v_color.y, v_color.z, 1.0f);
        }
);


TestShaderTmp::TestShaderTmp() {
    this->load();
    mvpUniformLocation();
}

bool TestShaderTmp::load() {

    std::string vShader = std::string("#version 330 core \n") + std::string(vertexShaderSource2);
    std::string fShader = std::string("#version 330 core \n") + std::string(fragmentShaderSource2);

    _programID = loadProgram_tmp(reinterpret_cast<const char *>(vShader.c_str()),
                                 reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void TestShaderTmp::useProgram() {
    glUseProgram(_programID);
}

GLuint TestShaderTmp::loadShader_tmp(GLenum type, const char *shaderSrc) {

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
        printf( "compiling log shader");
//        printf( "compiling log shader[%s:%d]:\n%s\n", _name.c_str(), type, infoLog );

        free(infoLog);
    }

    if ( !compiled ) {
        glDeleteShader ( shader );
        return 0;
    }
    return shader;
}

GLuint TestShaderTmp::loadProgram_tmp(const char *vertShaderSrc, const char *fragShaderSrc) {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    std::string vsCode, fsCode;

    vsCode = vertShaderSrc;
    fsCode = fragShaderSrc;

    // Load the vertex/fragment shaders
    vertexShader = loadShader_tmp(GL_VERTEX_SHADER, vsCode.c_str());
    if (vertexShader == 0) {
        return 0;
    }

    fragmentShader = loadShader_tmp(GL_FRAGMENT_SHADER, fsCode.c_str());
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
