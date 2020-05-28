//
//  model_loading.cpp
//  LearnOpenGL
//
//  Created by apple on 2019/8/14.
//  Copyright © 2019 apple. All rights reserved.
//

#include "model_loading.hpp"
#include "model.h"
#include "camera.h"
#include "shader.h"

void model_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void model_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void model_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void model_processInput(GLFWwindow *window);

// settings
const unsigned int MODEL_SCR_WIDTH = 800;
const unsigned int MODEL_SCR_HEIGHT = 600;

// camera
Camera model_camera(glm::vec3(0.0f, 0.0f, 3.0f));
float model_lastX = MODEL_SCR_WIDTH / 2.0f;
float model_lastY = MODEL_SCR_HEIGHT / 2.0f;
bool model_firstMouse = true;

// timing
float model_deltaTime = 0.0f;
float model_lastFrame = 0.0f;

int model_load() {
    //    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(MODEL_SCR_WIDTH, MODEL_SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, model_framebuffer_size_callback);
    glfwSetCursorPosCallback(window, model_mouse_callback);
    glfwSetScrollCallback(window, model_scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    Shader model_shader("./GLSL/3.model_loading/1.model_loading.vs", "./GLSL/3.model_loading/1.model.loading.fs");
    
    Model model_model("./resources/objects/nanosuit/nanosuit.obj");
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        model_deltaTime = currentFrame - model_lastFrame;
        model_lastFrame = currentFrame;
        
        // input
        // -----
        model_processInput(window);
        
        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // don't forget to enable shader before setting uniforms
        model_shader.use();
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(model_camera.Zoom), (float)MODEL_SCR_WIDTH / (float)MODEL_SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = model_camera.getViewMatrix();
        model_shader.setMat4("projection", projection);
        model_shader.setMat4("view", view);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        model_shader.setMat4("model", model);
        model_model.draw(model_shader);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}


void model_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void model_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (model_firstMouse)
    {
        model_lastX = xpos;
        model_lastY = ypos;
        model_firstMouse = false;
    }
    
    float xoffset = xpos - model_lastX;
    float yoffset = model_lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    model_lastX = xpos;
    model_lastY = ypos;
    
    model_camera.processMouseMovement(xoffset, yoffset);
}
void model_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    model_camera.processMouseScroll(yoffset);
}
void model_processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        model_camera.processKeyboard(FORWARD, model_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        model_camera.processKeyboard(BACKWARD, model_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        model_camera.processKeyboard(LEFT, model_deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        model_camera.processKeyboard(RIGHT, model_deltaTime);
}


unsigned int textureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
