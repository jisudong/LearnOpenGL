//
//  materials.cpp
//  testgl
//
//  Created by apple on 2019/5/27.
//  Copyright © 2019 自动化. All rights reserved.
//

#include "materials.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"

const unsigned int MATERIALS_SCR_WIDTH = 800;
const unsigned int MATERIALS_SCR_HEIGHT = 600;

void materials_framebuffer_callback(GLFWwindow *window, int width, int height);
void materials_input_process(GLFWwindow *window);
void materials_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void materials_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

Camera materials_camera(glm::vec3(0.0f, 0.0f, 3.0f));

float materials_deltaTime = 0.0f;
float materials_lastFrame = 0.0f;

float materials_lastX = MATERIALS_SCR_WIDTH / 2.0f;
float materials_lastY = MATERIALS_SCR_HEIGHT / 2.0f;
bool materials_firstMouse = true;

glm::vec3 materials_lampPos(0.8f, 0.4f, 1.5f);

int materials() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    GLFWwindow *window = glfwCreateWindow(MATERIALS_SCR_WIDTH, MATERIALS_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, materials_framebuffer_callback);
    glfwSetCursorPosCallback(window, materials_mouse_callback);
    glfwSetScrollCallback(window, materials_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader cubeShader("./GLSL/2.lighting/3.1.materials.vs", "./GLSL/2.lighting/3.1.materials.fs");
    Shader lampShader("./GLSL/2.lighting/3.1.lamp.vs", "./GLSL/2.lighting/3.1.lamp.fs");
    
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
    
    unsigned int VBO, cubeVAO;
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
        materials_deltaTime = currentFrame - materials_lastFrame;
        materials_lastFrame = currentFrame;
        
        materials_input_process(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        cubeShader.use();
        cubeShader.setVec3("light.position", materials_lampPos);
        cubeShader.setVec3("viewPos", materials_camera.Position);
        
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        cubeShader.setVec3("light.ambient", ambientColor);
        cubeShader.setVec3("light.diffuse", diffuseColor);
        cubeShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        
        cubeShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        cubeShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        cubeShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        cubeShader.setFloat("material.shininess", 32.0f);
        
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(materials_camera.Zoom), (float)MATERIALS_SCR_WIDTH/(float)MATERIALS_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = materials_camera.getViewMatrix();
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, -1.0f, 0.0f));
        cubeShader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, materials_lampPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", model);
        glBindVertexArray(lampVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &lampVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}

void materials_framebuffer_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}
void materials_input_process(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        materials_camera.processKeyboard(FORWARD, materials_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        materials_camera.processKeyboard(BACKWARD, materials_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        materials_camera.processKeyboard(LEFT, materials_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        materials_camera.processKeyboard(RIGHT, materials_deltaTime);
    }
}
void materials_mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (materials_firstMouse) {
        materials_lastX = xpos;
        materials_lastY = ypos;
        materials_firstMouse = false;
    }
    
    float xoffset = xpos - materials_lastX;
    float yoffset = materials_lastY - ypos;
    materials_lastX = xpos;
    materials_lastY = ypos;
    
    materials_camera.processMouseMovement(xoffset, yoffset);
}
void materials_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    materials_camera.processMouseScroll(yoffset);
}
