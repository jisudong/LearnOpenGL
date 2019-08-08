//
//  basic_lighting.cpp
//  testgl
//
//  Created by 秀健身 on 2019/5/22.
//  Copyright © 2019 自动化. All rights reserved.
//

#include "basic_lighting.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

void basic_lighting_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void basic_lighting_process_input(GLFWwindow *window);
void basic_lighting_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void basic_lighting_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

const unsigned int BASIC_LIGHTING_SCR_WIDTH = 800;
const unsigned int BASIC_LIGHTING_SCR_HEIGHT = 600;

Camera basic_lighting_camera(glm::vec3(0.0f, 0.0f, 4.0f));
float basic_lighting_lastX = BASIC_LIGHTING_SCR_WIDTH / 2.0f;
float basic_lighting_lastY = BASIC_LIGHTING_SCR_HEIGHT / 2.0f;
bool basic_lighting_firstMouse = true;

float basic_lighting_deltaTime = 0.0f;
float basic_lighting_lastFrame = 0.0f;

glm::vec3 basic_lighting_lampPos(0.8f, 0.4f, 1.5f);

int lighting() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(BASIC_LIGHTING_SCR_WIDTH, BASIC_LIGHTING_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, basic_lighting_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, basic_lighting_mouse_callback);
    glfwSetScrollCallback(window, basic_lighting_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader cubeShader("./GLSL/2.lighting/2.2.basic_lighting.vs", "./GLSL/2.lighting/2.2.basic_lighting.fs");
    Shader lampShader("./GLSL/2.lighting/2.1.lamp.vs", "./GLSL/2.lighting/2.1.lamp.fs");
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    unsigned int cubeVAO, VBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        basic_lighting_deltaTime = currentFrame - basic_lighting_lastFrame;
        basic_lighting_lastFrame = currentFrame;
        
        basic_lighting_process_input(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        basic_lighting_lampPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        basic_lighting_lampPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
        
        cubeShader.use();
        cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        cubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        cubeShader.setVec3("lightPos", basic_lighting_lampPos);
        cubeShader.setVec3("viewPos", basic_lighting_camera.Position);
        
        glm::mat4 projection = glm::perspective(glm::radians(basic_lighting_camera.Zoom), (float)BASIC_LIGHTING_SCR_WIDTH/(float)BASIC_LIGHTING_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = basic_lighting_camera.getViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(20.0f), vec3(1.0f, -1.0f, 0.0f));
        cubeShader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, basic_lighting_lampPos);
        model = glm::scale(model, glm::vec3(0.2));
        lampShader.setMat4("model", model);
        glBindVertexArray(lampVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lampVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}

void basic_lighting_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void basic_lighting_process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        basic_lighting_camera.processKeyboard(FORWARD, basic_lighting_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        basic_lighting_camera.processKeyboard(BACKWARD, basic_lighting_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        basic_lighting_camera.processKeyboard(LEFT, basic_lighting_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        basic_lighting_camera.processKeyboard(RIGHT, basic_lighting_deltaTime);
    }
}
void basic_lighting_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (basic_lighting_firstMouse) {
        basic_lighting_lastX = xpos;
        basic_lighting_lastY = ypos;
        basic_lighting_firstMouse = false;
    }
    
    double xoffset = xpos - basic_lighting_lastX;
    double yoffset = basic_lighting_lastY - ypos;
    basic_lighting_lastX = xpos;
    basic_lighting_lastY = ypos;
    
    basic_lighting_camera.processMouseMovement(xoffset, yoffset);
}
void basic_lighting_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    basic_lighting_camera.processMouseScroll(yoffset);
}
