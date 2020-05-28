//
//  camera_c.cpp
//  LearnOpenGL
//
//  Created by apple on 2019/8/7.
//  Copyright © 2019 apple. All rights reserved.
//

#include "camera_c.hpp"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"

void camera_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void camera_processInput(GLFWwindow *window);
void camera_mouse_callback(GLFWwindow *window, double xpos, double ypos);
void camera_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

const unsigned int CAMERA_SCR_WIDTH = 800;
const unsigned int CAMERA_SCR_HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float camera_deltaTime = 0.0f;
float camera_lastFrame = 0.0f;

Camera camera_camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool camera_firstMouse = true;
//float yaw       = -90.0f;
//float pitch     = 0.0f;
float camera_lastX     = 800.0f / 2.0;
float camera_lastY     = 600.0f / 2.0;
//float fov       = 45.0f;

int func_camera() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(CAMERA_SCR_WIDTH, CAMERA_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, camera_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, camera_mouse_callback);
    glfwSetScrollCallback(window, camera_scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    Shader ourShader("./GLSL/1.getting_started/7.1.camera.vs", "./GLSL/1.getting_started/7.1.camera.fs");
    
    float vertices[] = {
        //---- 位置 ----       - 纹理坐标 -
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3 (2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("./resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
    
    glm::mat4 projection = glm::perspective(glm::radians(camera_camera.Zoom), (float)CAMERA_SCR_WIDTH / (float)CAMERA_SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);
    
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = glfwGetTime();
        camera_deltaTime = currentFrame - camera_lastFrame;
        camera_lastFrame = currentFrame;
        
        camera_processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        ourShader.use();
        glBindVertexArray(VAO);
        
        //        glm::mat4 view = glm::mat4(1.0f);
        //        float radius = 10.0f;
        //        float camX = sin(glfwGetTime()) * radius;
        //        float camZ = cos(glfwGetTime()) * radius;
        //        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //        ourShader.setMat4("view", view);
        
        //        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 view = camera_camera.getViewMatrix();
        ourShader.setMat4("view", view);
        
        for (int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20 * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(0.2f, 0.3f * (i % 4), 0.5f));
            ourShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return 0;
}


void camera_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void camera_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    //    float cameraSpeed = 2.5 * camera_deltaTime;
    //    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    //        cameraPos += cameraSpeed * cameraFront;
    //    }
    //    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    //        cameraPos -= cameraSpeed * cameraFront;
    //    }
    //    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    //        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //    }
    //    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    //        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    //    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_camera.processKeyboard(FORWARD, camera_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_camera.processKeyboard(BACKWARD, camera_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_camera.processKeyboard(LEFT, camera_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_camera.processKeyboard(RIGHT, camera_deltaTime);
    }
}

void camera_mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (camera_firstMouse) {
        camera_lastX = xpos;
        camera_lastY = ypos;
        camera_firstMouse = false;
    }
    
    float xoffset = xpos - camera_lastX;
    float yoffset = camera_lastY - ypos;
    camera_lastX = xpos;
    camera_lastY = ypos;
    
    camera_camera.processMouseMovement(xoffset, yoffset);
    
    //    float sensitivity = 0.1f;
    //    xoffset *= sensitivity;
    //    yoffset *= sensitivity;
    //
    //    yaw += xoffset;
    //    pitch += yoffset;
    //
    //    if (pitch > 89.0f) {
    //        pitch = 89.0f;
    //    }
    //    if (pitch < -89.0f) {
    //        pitch = -89.0f;
    //    }
    //
    //    glm::vec3 front;
    //    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    //    front.y = sin(glm::radians(pitch));
    //    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //    cameraFront = glm::normalize(front);
}

void camera_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    //    if (fov >= 1.0f && fov <= 45.0f) {
    //        fov -= yoffset;
    //    }
    //    if (fov <= 1.0f) {
    //        fov = 1.0f;
    //    }
    //    if (fov >= 45.0f) {
    //        fov = 45.0f;
    //    }
    camera_camera.processMouseScroll(yoffset);
}
