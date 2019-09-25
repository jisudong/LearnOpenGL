//
//  advanced_lighting.cpp
//  LearnOpenGL
//
//  Created by 秀健身 on 2019/9/25.
//  Copyright © 2019 秀健身. All rights reserved.
//

#include "advanced_lighting.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"
#include "stb_image.h"



void al_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void al_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void al_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void al_inputProcess(GLFWwindow *window);
unsigned int al_loadTexture(const char *path);

const unsigned int AL_SCR_WIDTH = 1280;
const unsigned int AL_SCR_HEIGHT = 720;
bool blinn = false;
bool blinnKeyPressed = false;

Camera al_camera(glm::vec3(0.0f, 0.0f, 3.0f));
double al_lastX = (float)AL_SCR_WIDTH / 2.0;
double al_lastY = (float)AL_SCR_HEIGHT / 2.0;
bool al_firstMouse = true;

double al_deltaTime = 0.0f;
double al_lastFrame = 0.0f;

int blinnPhong() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(AL_SCR_WIDTH, AL_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, al_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, al_mouse_callback);
    glfwSetScrollCallback(window, al_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader shader("/Users/xiujianshen/Documents/GLSL/AdvancedLighting/1.advanced_lighting.vs", "/Users/xiujianshen/Documents/GLSL/AdvancedLighting/1.advanced_lighting.fs");
    
    float planeVertices[] = {
        // positions            // normals         // texcoords
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
        
        10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
        10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glBindVertexArray(0);
    
    unsigned int floorTexture = al_loadTexture("/Users/xiujianshen/Documents/resources/wood.png");
    
    shader.use();
    shader.setInt("texture1", 0);
    
    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
    
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        al_deltaTime = currentFrame - al_lastFrame;
        al_lastFrame = currentFrame;
        
        al_inputProcess(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)AL_SCR_WIDTH / (float)AL_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = al_camera.getViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
        shader.setVec3("viewPos", al_camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setInt("blinn", blinn);
        
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    
    glfwTerminate();
    return 0;
}

void al_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void al_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (al_firstMouse) {
        al_lastX = xpos;
        al_lastY = ypos;
        al_firstMouse = false;
    }
    
    double xoffset = xpos - al_lastX;
    double yoffset = al_lastY - ypos;
    al_lastX = xpos;
    al_lastY = ypos;
    al_camera.processMouseMovement(xoffset, yoffset);
}
void al_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    al_camera.processMouseScroll(yoffset);
}
void al_inputProcess(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        al_camera.processKeyboard(FORWARD, al_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        al_camera.processKeyboard(BACKWARD, al_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        al_camera.processKeyboard(LEFT, al_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        al_camera.processKeyboard(RIGHT, al_deltaTime);
    }
    
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed) {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        blinnKeyPressed = false;
    }
}

unsigned int al_loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        } else if (nrComponents == 3) {
            format = GL_RGB;
        } else if (nrComponents == 4) {
            format = GL_RGBA;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    
    stbi_image_free(data);
    
    return textureID;
}
