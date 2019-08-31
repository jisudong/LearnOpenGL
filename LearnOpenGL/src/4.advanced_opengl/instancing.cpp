//
//  instancing.cpp
//  LearnOpenGL
//
//  Created by 秀健身 on 2019/8/31.
//  Copyright © 2019 秀健身. All rights reserved.
//

#include "instancing.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include "camera.h"


void instance_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void instance_processInput(GLFWwindow *window);
void instance_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void instance_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

unsigned int INSTANCE_SCR_WIDTH = 1280;
unsigned int INSTANCE_SCR_HEIGHT = 720;

Camera instance_camera(glm::vec3(0.0f, 0.0f, 155.0f));
float instance_lastX = (float)INSTANCE_SCR_WIDTH / 2.0f;
float instance_lastY = (float)INSTANCE_SCR_HEIGHT / 2.0f;
bool instance_firstMouse = true;

double instance_deltaTime = 0.0f;
double instance_lastFrame = 0.0f;

int instanceArray() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(INSTANCE_SCR_WIDTH, INSTANCE_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, instance_framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("./GLSL/4.advanced_opengl/10.1.instancing.vs", "./GLSL/4.advanced_opengl/10.1.instancing.fs");
    
    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (float)x / 10 + offset;
            translation.y = (float)y / 10 + offset;
            translations[index++] = translation;
        }
    }
    
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
        
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.
    
    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        instance_processInput(window);
        
        shader.use();
        glBindVertexArray(quadVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
        glBindVertexArray(0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &instanceVBO);
    
    glfwTerminate();
    return 0;
}




void instance_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void instance_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        instance_camera.processKeyboard(FORWARD, instance_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        instance_camera.processKeyboard(BACKWARD, instance_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        instance_camera.processKeyboard(LEFT, instance_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        instance_camera.processKeyboard(RIGHT, instance_deltaTime);
}

void instance_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (instance_firstMouse) {
        instance_lastX = xpos;
        instance_lastY = ypos;
        instance_firstMouse = false;
    }
    
    float xoffset = xpos - instance_lastX;
    float yoffset = instance_lastY - ypos;
    
    instance_lastX = xpos;
    instance_lastY = ypos;
    
    instance_camera.processMouseMovement(xoffset, yoffset);
}
void instance_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    instance_camera.processMouseScroll(yoffset);
}
