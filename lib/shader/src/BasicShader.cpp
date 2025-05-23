#include "BasicShader.hpp"

const char* vertexBasicShader = R(
        layout (location = 0) in vec3 a_position;
        layout (location = 1) in vec3 a_color;
        layout (location = 2) in vec3 a_normal;

        uniform mat4 u_worldMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;
        uniform mat4 u_worldNormalMat;

        out vec3 v_color;
        out vec3 v_normal;

        void main()
        {
            vec4 normal = u_worldNormalMat * vec4(a_normal.xyz, 0.0);
            v_normal = normalize(normal.xyz);
            v_color = a_color;
            gl_Position = u_projMat * u_viewMat * u_worldMat * vec4(a_position.xyz, 1.0);
        }
);

const char* fragmentBasicShader = R(
        in vec3 v_color;
        in vec3 v_normal;
        out vec4 FragColor;

        void main()
        {
            float s = dot(normalize(v_normal), vec3(0, 0, 1));
            FragColor = vec4(v_color.rgb * s, 1.0f);
        }
);


BasicShader::BasicShader() {
    this->load();
    basicUniformLoc();
}

bool BasicShader::load() {
    string vShader = string("#version 330 core \n") + string(vertexBasicShader);
    string fShader = string("#version 330 core \n") + string(fragmentBasicShader);

    _programID = loadProgram(reinterpret_cast<const char *>(vShader.c_str()),
                             reinterpret_cast<const char *>(fShader.c_str()));

    assert(_programID != 0);

    return true;
}

void BasicShader::useProgram() {
    glUseProgram(_programID);
}

GLuint BasicShader::loadShader(GLenum type, const char *shaderSrc) {
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
        printf( "compiling log shader[%d]:\n%s\n", type, infoLog );

        free(infoLog);
    }

    if ( !compiled ) {
        glDeleteShader ( shader );
        return 0;
    }
    return shader;
}

GLuint BasicShader::loadProgram(const char *vertShaderSrc, const char *fragShaderSrc) {
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
