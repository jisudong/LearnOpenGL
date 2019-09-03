//
//  anti_aliasing.cpp
//  LearnOpenGL
//
//  Created by 秀健身 on 2019/9/3.
//  Copyright © 2019 秀健身. All rights reserved.
//

#include "anti_aliasing.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint screenWidth = 800, screenHeight = 600;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void msaa_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void msaa_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void Do_Movement();

Camera msaa_camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat msaa_lastX = 400, msaa_lastY = 300;
bool msaa_firstMouse = true;

GLfloat msaa_deltaTime = 0.0f;
GLfloat msaa_lastFrame = 0.0f;

int MSAA() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, msaa_mouse_callback);
    glfwSetScrollCallback(window, msaa_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glViewport(0, 0, screenWidth, screenHeight);
    
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("./GLSL/4.advanced_opengl/11.1.MSAA.vs", "./GLSL/4.advanced_opengl/11.1.MSAA.fs");
    
    GLfloat cubeVertices[] = {
        // Positions
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
        -0.5f,  0.5f, -0.5f
    };
    
    // Setup cube VAO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        msaa_deltaTime = currentFrame - msaa_lastFrame;
        msaa_lastFrame = currentFrame;
        
        glfwPollEvents();
        Do_Movement();
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(msaa_camera.Zoom, (GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(msaa_camera.getViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}
void msaa_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(msaa_firstMouse)
    {
        msaa_lastX = xpos;
        msaa_lastY = ypos;
        msaa_firstMouse = false;
    }
    
    GLfloat xoffset = xpos - msaa_lastX;
    GLfloat yoffset = msaa_lastY - ypos;
    
    msaa_lastX = xpos;
    msaa_lastY = ypos;
    
    msaa_camera.processMouseMovement(xoffset, yoffset);
}
void msaa_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    msaa_camera.processMouseScroll(yoffset);
}
void Do_Movement()
{
    if (keys[GLFW_KEY_W]) {
        msaa_camera.processKeyboard(FORWARD, msaa_deltaTime);
    }
    if (keys[GLFW_KEY_S]) {
        msaa_camera.processKeyboard(BACKWARD, msaa_deltaTime);
    }
    if (keys[GLFW_KEY_A]) {
        msaa_camera.processKeyboard(LEFT, msaa_deltaTime);
    }
    if (keys[GLFW_KEY_D]) {
        msaa_camera.processKeyboard(RIGHT, msaa_deltaTime);
    }
}
