#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const char *vertexShaderSource =
    R"(#version 330 core
layout (location = 0) in vec2 position;            
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;
void main(){
    texCoord = inTexCoord;
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
})";

const char *fragmentShaderSource =
    R"(#version 330 core
in vec2 TexCoords;
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;
#define PI 3.14159265359
#define TWOPI 6.28318530718
/*
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 */

// Color computation inspired by: https://www.shadertoy.com/view/Ms2SD1

// I was looking for waves that would look good on a flat 2D plane.
// I didn't really find something to my liking so I tinkered a bit
// (with some inspiration from other shaders obviously).
// I thought the end result was decent so I wanted to share

const float gravity = 1.0;
const float waterTension = 0.01;
const vec3 skyCol1 = vec3(0.2, 0.4, 0.6);
const vec3 skyCol2 = vec3(0.4, 0.7, 1.0);
const vec3 sunCol  =  vec3(8.0,7.0,6.0)/8.0;
const vec3 seaCol1 = vec3(0.1,0.2,0.2);
const vec3 seaCol2 = vec3(0.8,0.9,0.6);

float gravityWave(in vec2 p, float k, float h) {
  float w = sqrt(gravity*k*tanh(k*h));
  return sin(p.y*k + w*iTime);
}

float capillaryWave(in vec2 p, float k, float h) {
  float w = sqrt((gravity*k + waterTension*k*k*k)*tanh(k*h));
  return sin(p.y*k + w*iTime);
}

void rot(inout vec2 p, float a) {
  float c = cos(a);
  float s = sin(a);
  p = vec2(p.x*c + p.y*s, -p.x*s + p.y*c);
}

float seaHeight(in vec2 p) {
  float height = 0.0;

  float k = 1.0;
  float kk = 1.3;
  float a = 0.25;
  float aa = 1.0/(kk*kk);

  float h = 10.0;
  p *= 0.5;

  for (int i = 0; i < 3; ++i) {
    height += a*gravityWave(p + float(i), k, h);
    rot(p, float(i));
    k *= kk;
    a *= aa;
  }
  
  for (int i = 3; i < 7; ++i) {
    height += a*capillaryWave(p + float(i), k, h);
    rot(p, float(i));
    k *= kk;
    a *= aa;
  }

  return height;
}

vec3 seaNormal(in vec2 p, in float d) {
  vec2 eps = vec2(0.001*pow(d, 1.5), 0.0);
  vec3 n = vec3(
    seaHeight(p + eps) - seaHeight(p - eps),
    2.0*eps.x,
    seaHeight(p + eps.yx) - seaHeight(p - eps.yx)
  );
  
  return normalize(n);
}

vec3 sunDirection() {
  vec3 dir = normalize(vec3(0, 0.15, 1));
  return dir;
}

vec3 skyColor(vec3 rd) {
  vec3 sunDir = sunDirection();

  float sunDot = max(dot(rd, sunDir), 0.0);
  
  vec3 final = vec3(0.0);

  final += mix(skyCol1, skyCol2, rd.y);

  final += 0.5*sunCol*pow(sunDot, 20.0);

  final += 4.0*sunCol*pow(sunDot, 400.0);
    
  return final;
}

void main() {
  vec2 q=fragCoord.xy/iResolution.xy; 
  vec2 p = -1.0 + 2.0*q;
  p.x *= iResolution.x/iResolution.y;

  vec3 ro = vec3(0.0, 10.0, 0.0);
  vec3 ww = normalize(vec3(0.0, -0.1, 1.0));
  vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww));
  vec3 vv = normalize(cross(ww,uu));
  vec3 rd = normalize(p.x*uu + p.y*vv + 2.5*ww);

  vec3 col = vec3(0.0);

  float dsea = (0.0 - ro.y)/rd.y;
  
  vec3 sunDir = sunDirection();
  
  vec3 sky = skyColor(rd);
    
  if (dsea > 0.0) {
    vec3 p = ro + dsea*rd;
    float h = seaHeight(p.xz);
    vec3 nor = mix(seaNormal(p.xz, dsea), vec3(0.0, 1.0, 0.0), smoothstep(0.0, 200.0, dsea));
    float fre = clamp(1.0 - dot(-nor,rd), 0.0, 1.0);
    fre = pow(fre, 3.0);
    float dif = mix(0.25, 1.0, max(dot(nor,sunDir), 0.0));
    
    vec3 refl = skyColor(reflect(rd, nor));
    vec3 refr = seaCol1 + dif*seaCol2*0.1; 
    
    col = mix(refr, 0.9*refl, fre);
      
    float atten = max(1.0 - dot(dsea,dsea) * 0.001, 0.0);
    col += seaCol2 * (p.y - h) * 2.0 * atten;
      
    col = mix(col, sky, 1.0 - exp(-0.01*dsea));
  } else {
    col = sky;
  }

  fragColor = vec4(col,1.0);
})";

int main()
{
    int width = 800;
    int height = 600;

    glm::vec2 screen(width, height);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "OpenglContext", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "failed to create window" << std::endl;
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "failed to initialize glad with processes " << std::endl;
        exit(-1);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int samples = 4;
    float quadVerts[] = {
        -1.0, -1.0, 0.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, -1.0, 1.0, 0.0,

        1.0, -1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 1.0, 1.0};

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLuint texColor;
    glGenTextures(1, &texColor);
    glBindTexture(GL_TEXTURE_2D, texColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    glUniform2fv(glGetUniformLocation(shaderProgram, "iResolution"), 1, &screen[0]);

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), (int)currentFrame % 60);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    // cleanup
}