//
// Created by Vyacheslav on 30.03.2025.
//

#ifndef OPENGLTEST_SHADER_H
#define OPENGLTEST_SHADER_H

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <filesystem.h>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath, const char* geomPath)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geomCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            gShaderFile.open(geomPath);
            std::stringstream vShaderStream, fShaderStream, gShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            gShaderStream << gShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();
            gShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            geomCode = gShaderStream.str();
        }
        catch(std::ifstream::failure e) {
            std::cout << "Error reading shader file.\n" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geomCode.c_str();

        unsigned int vertex, fragment, geometry, tess_control, tess_eval;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "Error compiling vertex shader. Info log:\n" <<
                    infoLog << std::endl;
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "Error compiling fragment shader. Info log:\n" <<
                      infoLog << std::endl;
        }

        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout << "Error compiling geometry shader. Info log:\n" <<
                      infoLog << std::endl;
        }


        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glAttachShader(ID, geometry);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Error linking shader program. Info log:\n" <<
                    infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(geometry);
    }

    void use()
    {
        glUseProgram(ID);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string &name, glm::vec2 value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string &name, glm::vec3 value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

namespace SrcXShader {
    struct XShader {
        const char *shaderPath;
        const char *shaderCode;
    };

    XShader loadVertexShader(const char *vertexShaderPath) {
        std::string vertexCode;
        std::ifstream vShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vShaderFile.open(vertexShaderPath);
            std::stringstream vShaderStream;

            vShaderStream << vShaderFile.rdbuf();

            vShaderFile.close();

            vertexCode = vShaderStream.str();
        }
        catch (std::ifstream::failure e) {
            throw "SrcXEngine: error reading vertex shader file.";
        }
        const char *vShaderCode = vertexCode.c_str();
    }

    XShader assignVertexShader(XShader vertexShader) {
        unsigned int vertex;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShader.shaderCode, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            throw "SrcXEngine: error compiling vertex shader.";
            std::cout << infoLog << std::endl;
        }
    }

    XShader loadFragmentShader(const char *fragmentShaderPath) {
        std::string fragmentCode;
        std::ifstream fShaderFile;

        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            fShaderFile.open(fragmentShaderPath);
            std::stringstream fShaderStream;

            fShaderStream << fShaderFile.rdbuf();

            fShaderFile.close();

            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e) {
            throw "SrcXEngine: error reading fragment shader file.";
        }
        const char *fShaderCode = fragmentCode.c_str();
    }

    XShader assignFragmentShader(XShader fragmentShader) {
        unsigned int fragment;
        int success;
        char infoLog[512];

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShader.shaderCode, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            throw "SrcXEngine: error compiling fragment shader.";
            std::cout << infoLog << std::endl;
        }
    }
}

#endif //OPENGLTEST_SHADER_H
