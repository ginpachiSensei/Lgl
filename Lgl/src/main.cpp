#include "lgl.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "glError.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);
int main()
{
    glm::vec2 screen(mWidth, mHeight);
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // window creation
    GLFWwindow *mWindow = glfwCreateWindow(mWidth, mHeight, "LearnOpenGL", NULL, NULL);
    if (mWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);
    // for window resizing
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    // enable OpenGL debug context if context allows for debug context
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // makes sure errors are displayed synchronously
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // cursor disabled
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int sample = 4;
    float quadVerts[] = {
        -1.0, -1.0, 0.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 0.0,

        1.0, -1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0};

    // vertex buffer object
    // Mirage::VertexBuffer VBO(vertices, sizeof(vertices));

    // shader object
    Mirage::Shader shader;
    shader.attach("main.frag").attach("main.vert");
    shader.link();
    shader.activate();

    // vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex buffer object
    Mirage::VertexBuffer VBO(quadVerts, sizeof(quadVerts));

    // Mirage::IndexBuffer IBO(indices, 6);
    // IBO.Bind();

    // vertex attribute object
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint texColor;
    glGenTextures(1, &texColor);
    glBindTexture(GL_TEXTURE_2D, texColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader.bind("iResolution", screen[0]);
    glUniform2fv(shader.getUniLoc("iResolution"), 1, &screen[0]);

    // main loop
    while (!glfwWindowShouldClose(mWindow))
    {
        // input
        // -----
        processInput(mWindow);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        // {
        //     glfwSetWindowShouldClose(mWindow, true);
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glUseProgram(shaderProgram);
        shader.activate();
        // glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (int)currentFrame % 60);
        shader.bind("iTime", (int)currentFrame % 60);
        // glUniform2fv(shader.getUniLoc("iTime"), (int)currentFrame % 60);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    shader.~Shader();
    VBO.~VertexBuffer();
    // IBO.~IndexBuffer();
    glfwTerminate();
    return 0;
}