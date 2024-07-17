#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Engine.hpp"
#include "UXUtil.hpp"

constexpr unsigned int SCR_WIDTH = 1280 * 1.2;
constexpr unsigned int SCR_HEIGHT = 720 * 1.2;

RenderEngine* engine = nullptr;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto dir = UXUtil::ViewSpaceMoveDirection::NOTHING;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir = UXUtil::ViewSpaceMoveDirection::FORWARD;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir = UXUtil::ViewSpaceMoveDirection::BACKWARD;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir = UXUtil::ViewSpaceMoveDirection::LEFT;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir = UXUtil::ViewSpaceMoveDirection::RIGHT;

    UXUtil::updateViewPosition(dir, deltaTime, engine);
}

void mouse_callback(GLFWwindow* window, double x, double y) {
    UXUtil::updateViewRotation(float(x), float(y), engine);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    engine->setScreenSize(w, h);
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RenderEngine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //setup...
    UXUtil::setInitialMousePosition(SCR_WIDTH / 2.0, SCR_HEIGHT / 2.0);

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    engine = new RenderEngine(SceneType::SampleScene0);
    engine->prepare();
    engine->setScreenSize(viewport[2], viewport[3]);

    //render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        engine->render();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    delete engine;
    return 0;
}