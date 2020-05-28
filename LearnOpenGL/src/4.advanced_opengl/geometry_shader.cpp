//
//  geometry_shader.cpp
//  LearnOpenGL
//
//  Created by apple on 2019/8/30.
//  Copyright © 2019 apple. All rights reserved.
//

#include "geometry_shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

void geometry_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void geometry_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void geometry_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void geometry_processInput(GLFWwindow *window);

const unsigned int GEOMETRY_SCR_WIDTH = 1280;
const unsigned int GEOMETRY_SCR_HEIGHT = 720;

Camera geometry_camera(glm::vec3(0.0f, 0.0f, 3.0f));
double geometry_lastX = (float)GEOMETRY_SCR_WIDTH / 2.0;
double geometry_lastY = (float)GEOMETRY_SCR_HEIGHT / 2.0;
bool geometry_firstMouse = true;

double geometry_deltaTime = 0.0f;
double geometry_lastFrame = 0.0f;


int geometry() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(GEOMETRY_SCR_WIDTH, GEOMETRY_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("./GLSL/4.advanced_opengl/9.1.geometry.vs", "./GLSL/4.advanced_opengl/9.1.geometry.fs", "./GLSL/4.advanced_opengl/9.1.geometry.gs");
    
    float points[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
    };
    
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 4);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}


#pragma mark - 爆炸物体
int explode() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(GEOMETRY_SCR_WIDTH, GEOMETRY_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, geometry_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, geometry_mouse_callback);
    glfwSetScrollCallback(window, geometry_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("./GLSL/4.advanced_opengl/9.2.geometry.vs", "./GLSL/4.advanced_opengl/9.2.geometry.fs", "./GLSL/4.advanced_opengl/9.2.geometry.gs");
    
    Model nanosuit("./resources/objects/nanosuit/nanosuit.obj");
    
    while (!glfwWindowShouldClose(window)) {
        double currenFrame = glfwGetTime();
        geometry_deltaTime = currenFrame - geometry_lastFrame;
        geometry_lastFrame = currenFrame;
        
        geometry_processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(geometry_camera.Zoom), (float)GEOMETRY_SCR_WIDTH / (float)GEOMETRY_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = geometry_camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f));
        
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        
        shader.setFloat("time", glfwGetTime());
        
        nanosuit.draw(shader);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}


#pragma mark - 法线可视化
int normal_visualization() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(GEOMETRY_SCR_WIDTH, GEOMETRY_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, geometry_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, geometry_mouse_callback);
    glfwSetScrollCallback(window, geometry_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("./GLSL/4.advanced_opengl/9.3.default.vs", "./GLSL/4.advanced_opengl/9.2.geometry.fs");
    Shader normalShader("./GLSL/4.advanced_opengl/9.3.geometry.vs", "./GLSL/4.advanced_opengl/9.3.geometry.fs", "./GLSL/4.advanced_opengl/9.3.geometry.gs");
    
    Model nanosuit("./resources/objects/nanosuit/nanosuit.obj");
    
    while (!glfwWindowShouldClose(window)) {
        double currenFrame = glfwGetTime();
        geometry_deltaTime = currenFrame - geometry_lastFrame;
        geometry_lastFrame = currenFrame;
        
        geometry_processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(geometry_camera.Zoom), (float)GEOMETRY_SCR_WIDTH / (float)GEOMETRY_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = geometry_camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        //        model = glm::scale(model, glm::vec3(0.2f));
        
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);
        
        nanosuit.draw(shader);
        
        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);
        
        nanosuit.draw(normalShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}




void geometry_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void geometry_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (geometry_firstMouse) {
        geometry_lastX = xpos;
        geometry_lastY = ypos;
        geometry_firstMouse = false;
    }
    
    double xoffset = xpos - geometry_lastX;
    double yoffset = geometry_lastY - ypos;
    geometry_lastX = xpos;
    geometry_lastY = ypos;
    
    geometry_camera.processMouseMovement(xoffset, yoffset);
}
void geometry_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    geometry_camera.processMouseScroll(yoffset);
}
void geometry_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        geometry_camera.processKeyboard(FORWARD, geometry_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        geometry_camera.processKeyboard(BACKWARD, geometry_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        geometry_camera.processKeyboard(LEFT, geometry_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        geometry_camera.processKeyboard(RIGHT, geometry_deltaTime);
    }
}

