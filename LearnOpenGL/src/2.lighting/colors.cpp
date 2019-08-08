//
//  colors.cpp
//  testgl
//
//  Created by 秀健身 on 2019/5/21.
//  Copyright © 2019 自动化. All rights reserved.
//

#include "colors.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void colors_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void colors_process_input(GLFWwindow *window);
void colors_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void colors_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

const unsigned int COLORS_SCR_WIDTH = 800;
const unsigned int COLORS_SCR_HEIGHT = 600;

Camera colors_camera(glm::vec3(0.0f, 0.0f, 3.0f));
float colors_lastX = COLORS_SCR_WIDTH / 2.0f;
float colors_lastY = COLORS_SCR_HEIGHT / 2.0f;
bool colors_firstMouse = true;

float colors_deltaTime = 0.0f;
float colors_lastFrame = 0.0f;

glm::vec3 colors_lightPos(1.2f, 1.0f, 2.0f);


int colors() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(COLORS_SCR_WIDTH, COLORS_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, colors_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, colors_mouse_callback);
    glfwSetScrollCallback(window, colors_scroll_callback);
    
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader lightingShader("./GLSL/2.lighting/1.colors.vs", "./GLSL/2.lighting/1.colors.fs");
    Shader lampShader("./GLSL/2.lighting/1.lamp.vs", "./GLSL/2.lighting/1.lamp.fs");
    
    float vertices[] = {
        //---- 位置 ----
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
    
    unsigned int cubeVAO, VBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = glfwGetTime();
        colors_deltaTime = currentFrame - colors_lastFrame;
        colors_lastFrame = currentFrame;
        
        colors_process_input(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        
        glm::mat4 projection = glm::perspective(glm::radians(colors_camera.Zoom),  (float)COLORS_SCR_WIDTH / (float)COLORS_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = colors_camera.getViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, colors_lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", model);
        
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    
    return 0;
}

void colors_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void colors_process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        colors_camera.processKeyboard(FORWARD, colors_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        colors_camera.processKeyboard(BACKWARD, colors_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        colors_camera.processKeyboard(LEFT, colors_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        colors_camera.processKeyboard(RIGHT, colors_deltaTime);
}

void colors_mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (colors_firstMouse) {
        colors_lastX = xpos;
        colors_lastY = ypos;
        colors_firstMouse = false;
    }
    
    float xoffset = xpos - colors_lastX;
    float yoffset = colors_lastY - ypos;
    colors_lastX = xpos;
    colors_lastY = ypos;
    
    colors_camera.processMouseMovement(xoffset, yoffset);
}

void colors_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    colors_camera.processMouseScroll(yoffset);
}
