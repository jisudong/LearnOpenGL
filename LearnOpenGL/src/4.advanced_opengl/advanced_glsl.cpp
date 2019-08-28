//
//  advanced_glsl.cpp
//  LearnOpenGL
//
//  Created by 秀健身 on 2019/8/28.
//  Copyright © 2019 秀健身. All rights reserved.
//

#include "advanced_glsl.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include <iostream>


void glsl_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void glsl_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void glsl_processInput(GLFWwindow *window);


const unsigned int GLSL_SCR_WIDTH = 1280;
const unsigned int GLSL_SCR_HEIGHT = 720;

Camera glsl_camera(glm::vec3(0.0f, 0.0f, 3.0f));
float glsl_lastX = (float)GLSL_SCR_WIDTH / 2.0f;
float glsl_lastY = (float)GLSL_SCR_HEIGHT / 2.0f;
bool glsl_firstMouse = true;

float glsl_deltaTime = 0.0f;
float glsl_lastFrame = 0.0f;


int advancedGlsl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    GLFWwindow *window = glfwCreateWindow(GLSL_SCR_WIDTH, GLSL_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, glsl_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, glsl_mouse_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shaderRed("./GLSL/4.advanced_opengl/8.advanced_glsl.vs", "./GLSL/4.advanced_opengl/8.red.fs");
    Shader shaderGreen("./GLSL/4.advanced_opengl/8.advanced_glsl.vs", "./GLSL/4.advanced_opengl/8.green.fs");
    Shader shaderBlue("./GLSL/4.advanced_opengl/8.advanced_glsl.vs", "./GLSL/4.advanced_opengl/8.blue.fs");
    Shader shaderYellow("./GLSL/4.advanced_opengl/8.advanced_glsl.vs", "./GLSL/4.advanced_opengl/8.yellow.fs");
    
    float cubeVertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");
    
    glUniformBlockBinding(shaderRed.ID, uniformBlockIndexRed, 0);
    glUniformBlockBinding(shaderGreen.ID, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(shaderBlue.ID, uniformBlockIndexBlue, 0);
    glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);
    
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)GLSL_SCR_WIDTH / (float)GLSL_SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        glsl_deltaTime = currentFrame - glsl_lastFrame;
        glsl_lastFrame = currentFrame;
        
        glsl_processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 view = glsl_camera.getViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        glBindVertexArray(cubeVAO);
        shaderRed.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
        shaderRed.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        shaderGreen.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
        shaderGreen.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        shaderBlue.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
        shaderBlue.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        shaderYellow.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
        shaderYellow.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    
    glfwTerminate();
    return 0;
}


void glsl_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void glsl_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (glsl_firstMouse) {
        glsl_lastX = xpos;
        glsl_lastY = ypos;
        glsl_firstMouse = false;
    }
    
    double xoffset = xpos - glsl_lastX;
    double yoffset = glsl_lastY - ypos;
    glsl_lastX = xpos;
    glsl_lastY = ypos;
    
    glsl_camera.processMouseMovement(xoffset, yoffset);
}

void glsl_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glsl_camera.processKeyboard(FORWARD, glsl_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glsl_camera.processKeyboard(BACKWARD, glsl_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glsl_camera.processKeyboard(LEFT, glsl_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glsl_camera.processKeyboard(RIGHT, glsl_deltaTime);
    }
}
