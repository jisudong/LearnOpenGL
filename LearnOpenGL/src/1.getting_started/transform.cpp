//
//  transform.cpp
//  testgl
//
//  Created by 秀健身 on 2019/5/17.
//  Copyright © 2019 自动化. All rights reserved.
//

#include "transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "shader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void transform_framebuffer_size_callback(GLFWwindow *window, int width, int height);
void transform_processInput(GLFWwindow *window);

const unsigned int TRANSFORM_SCR_WIDTH = 800;
const unsigned int TRANSFORM_SCR_HEIGHT = 600;

int transform() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
#endif
    
    GLFWwindow *window = glfwCreateWindow(TRANSFORM_SCR_WIDTH, TRANSFORM_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, transform_framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }
    
    Shader ourShader("/Users/xiujianshen/Documents/GLSL/Getting_Started/5.1.transform.vs", "/Users/xiujianshen/Documents/GLSL/Getting_Started/5.1.transform.fs");
    
    float vertices[] = {
    //     ---- 位置 ----       - 纹理坐标 -
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // 右上
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f    // 左上
    };
    
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
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
    unsigned char *data = stbi_load("/Users/xiujianshen/Documents/resources/container.jpg", &width, &height, &nrChannels, 0);
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
    data = stbi_load("/Users/xiujianshen/Documents/resources/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
    
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
    
//    glm::mat4 trans = glm::mat4(1.0f);
//    trans = glm::rotate(trans, glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
//    trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
    
//    unsigned int tranformLoc = glGetUniformLocation(ourShader.ID, "transform");
//    glUniformMatrix4fv(tranformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    
    while (!glfwWindowShouldClose(window)) {
        transform_processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        
        ourShader.use();
        
        unsigned int tranformLoc = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(tranformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glm::mat4 transform2 = glm::mat4(1.0f);
        transform2 = glm::translate(transform2, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scaleAmount = fabs(sin((double)glfwGetTime()));
        transform2 = glm::scale(transform2, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        glUniformMatrix4fv(tranformLoc, 1, GL_FALSE, glm::value_ptr(transform2));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
    
}


void transform_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void transform_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
