//
//  shader_c.cpp
//  LearnOpenGL
//
//  Created by 秀健身 on 2019/8/5.
//  Copyright © 2019 秀健身. All rights reserved.
//

#include "shader_c.hpp"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"


using namespace std;

extern const unsigned int SHADER_SCR_WIDTH = 800;
extern const unsigned int SHADER_SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void shader_processInput(GLFWwindow *window);



const char *vertexShaderString = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"  //位置变量的属性位置值为 0
"layout (location = 1) in vec3 aColor;\n" //颜色变量的属性位置值为 1
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(aPos, 1.0f);\n"
"vertexColor = vec4(aColor, 1.0f);\n"
"}\0";

const char *fragmentShaderString = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"void main()\n"
"{\n"
"FragColor = vertexColor;\n"
"}\n\0";

const char *uniformShaderString = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"FragColor = ourColor;\n"
"}\n\0";

int inout() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    GLFWwindow *window = glfwCreateWindow(SHADER_SCR_WIDTH, SHADER_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        cout << "failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "failed initialize glad" << endl;
        return -1;
    }
    
    //    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
    //    glCompileShader(vertexShader);
    //
    //    int success;
    //    char infoLog[512];
    //    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //    if (!success) {
    //        cout << "failed to vertexShader compile" << endl;
    //        return -1;
    //    }
    //
    //    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
    //    glCompileShader(fragmentShader);
    //    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    //    if (!success) {
    //        cout << "failed to fragmentShader compile" << endl;
    //        return -1;
    //    }
    //
    //    int shaderProgram = glCreateProgram();
    //    glAttachShader(shaderProgram, vertexShader);
    //    glAttachShader(shaderProgram, fragmentShader);
    //    glLinkProgram(shaderProgram);
    //    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    //    if (!success) {
    //        cout << "failed to linking shaderProgram" << endl;
    //        return -1;
    //    }
    //    glDeleteShader(vertexShader);
    //    glDeleteShader(fragmentShader);

    Shader ourShader("./GLSL/1.getting_started/vertex.vs", "./GLSL/1.getting_started/fragment.fs");
    
    float vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    while (!glfwWindowShouldClose(window)) {
        shader_processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ourShader.use();
        //        glUseProgram(shaderProgram);
        
        //        double timeValue = glfwGetTime();
        //        float greenValue = sin(timeValue) / 2.0f + 0.5;
        //        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    
    glfwTerminate();
    
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void shader_processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
