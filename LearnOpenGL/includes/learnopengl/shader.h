//
//  shader.h
//  LearnOpenGL
//
//  Created by apple on 2019/8/3.
//  Copyright © 2019 apple. All rights reserved.
//

#ifndef shader_h
#define shader_h

#import <glad/glad.h>
#import <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>


class Shader {
public:
    unsigned int ID; // 着色器程序对象ID
    
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr) {
        
        // 1. 从文件路径中获取顶点/片段着色器
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        
        // 保证ifstream对象可以抛出异常：
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            // 打开文件
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            
            // 读取文件的缓冲内容到数据流中
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            
            // 关闭文件处理器
            vShaderFile.close();
            fShaderFile.close();
            
            // 转换数据流到string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            if (geometryPath != nullptr) {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
            
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        
        unsigned int vertex, fragment;
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        unsigned int geometry;
        if (geometryPath != nullptr) {
            const char *gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr) {
            glAttachShader(ID, geometry);
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr) {
            glDeleteShader(geometry);
        }
    }
    
    void use() {
        glUseProgram(ID);
    }
    
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setVec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    
    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    
    void setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
    void setVec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    
    void setMat2(const std::string &name, glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat3(const std::string &name, glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat4(const std::string &name, glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};



#endif /* shader_h */
