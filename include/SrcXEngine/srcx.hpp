//
// Created by Vyacheslav on 04.04.2025.
//

#ifndef OPENGLTEST_SRCX_HPP
#define OPENGLTEST_SRCX_HPP

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <SrcXEngine/shader.h>
#include <SrcXEngine/material.hpp>
#include <filesystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

struct XWindow {
    int width, height;
    const char* title;
    GLFWwindow* window;
    bool shouldClose = false;
};

struct XKey {
    int keyCode;
};

struct XTexture {
    std::string path;
    unsigned char *data;

    unsigned int width, height;
    unsigned int nrChannels;
};

class SrcX
{
public:
    enum KeyCode {
        SRCX_KEY_UNKNOWN = -1,
        SRCX_KEY_A = 65,
        SRCX_KEY_B = 66,
        SRCX_KEY_C = 67,
        SRCX_KEY_D = 68,
        SRCX_KEY_E = 69,
        SRCX_KEY_F = 70,
        SRCX_KEY_G = 71,
        SRCX_KEY_H = 72,
        SRCX_KEY_I = 73,
        SRCX_KEY_J = 74,
        SRCX_KEY_K = 75,
        SRCX_KEY_L = 76,
        SRCX_KEY_M = 77,
        SRCX_KEY_N = 78,
        SRCX_KEY_O = 79,
        SRCX_KEY_P = 80,
        SRCX_KEY_Q = 81,
        SRCX_KEY_R = 82,
        SRCX_KEY_S = 83,
        SRCX_KEY_T = 84,
        SRCX_KEY_U = 85,
        SRCX_KEY_V = 86,
        SRCX_KEY_W = 87,
        SRCX_KEY_X = 88,
        SRCX_KEY_Y = 89,
        SRCX_KEY_Z = 90,

        SRCX_KEY_0 = 48,
        SRCX_KEY_1 = 49,
        SRCX_KEY_2 = 50,
        SRCX_KEY_3 = 51,
        SRCX_KEY_4 = 52,
        SRCX_KEY_5 = 53,
        SRCX_KEY_6 = 54,
        SRCX_KEY_7 = 55,
        SRCX_KEY_8 = 56,
        SRCX_KEY_9 = 57,

        SRCX_KEY_SPACE = 32,
        SRCX_KEY_ENTER = 257,
        SRCX_KEY_ESCAPE = 256,
        SRCX_KEY_TAB = 258,
        SRCX_KEY_BACKSPACE = 259,
        SRCX_KEY_LEFT_SHIFT = 340,
        SRCX_KEY_RIGHT_SHIFT = 344,
        SRCX_KEY_LEFT_CONTROL = 341,
        SRCX_KEY_RIGHT_CONTROL = 345,
        SRCX_KEY_LEFT_ALT = 342,
        SRCX_KEY_RIGHT_ALT = 346,
        SRCX_KEY_LEFT_SUPER = 347,
        SRCX_KEY_RIGHT_SUPER = 348,

        SRCX_KEY_UP = 265,
        SRCX_KEY_DOWN = 264,
        SRCX_KEY_LEFT = 263,
        SRCX_KEY_RIGHT = 262,

        SRCX_KEY_F1 = 290,
        SRCX_KEY_F2 = 291,
        SRCX_KEY_F3 = 292,
        SRCX_KEY_F4 = 293,
        SRCX_KEY_F5 = 294,
        SRCX_KEY_F6 = 295,
        SRCX_KEY_F7 = 296,
        SRCX_KEY_F8 = 297,
        SRCX_KEY_F9 = 298,
        SRCX_KEY_F10 = 299,
        SRCX_KEY_F11 = 300,
        SRCX_KEY_F12 = 301,

        SRCX_KEY_NUM0 = 320,
        SRCX_KEY_NUM1 = 321,
        SRCX_KEY_NUM2 = 322,
        SRCX_KEY_NUM3 = 323,
        SRCX_KEY_NUM4 = 324,
        SRCX_KEY_NUM5 = 325,
        SRCX_KEY_NUM6 = 326,
        SRCX_KEY_NUM7 = 327,
        SRCX_KEY_NUM8 = 328,
        SRCX_KEY_NUM9 = 329,
        SRCX_KEY_NUM_DECIMAL = 330,
        SRCX_KEY_NUM_DIVIDE = 331,
        SRCX_KEY_NUM_MULTIPLY = 332,
        SRCX_KEY_NUM_SUBTRACT = 333,
        SRCX_KEY_NUM_ADD = 334,
        SRCX_KEY_NUM_ENTER = 335,
        SRCX_KEY_NUM_EQUAL = 336,

        SRCX_KEY_CAPS_LOCK = 280,
        SRCX_KEY_SCROLL_LOCK = 281,
        SRCX_KEY_PRINT_SCREEN = 283,
        SRCX_KEY_PAUSE = 284,
        SRCX_KEY_INSERT = 260,
        SRCX_KEY_HOME = 268,
        SRCX_KEY_PAGE_UP = 266,
        SRCX_KEY_PAGE_DOWN = 267,
        SRCX_KEY_END = 269,
        SRCX_KEY_DELETE = 261,

        SRCX_KEY_MEDIA_PLAY_PAUSE = 350,
        SRCX_KEY_MEDIA_STOP = 351,
        SRCX_KEY_MEDIA_NEXT_TRACK = 352,
        SRCX_KEY_MEDIA_PREV_TRACK = 353,
        SRCX_KEY_MEDIA_VOLUME_UP = 354,
        SRCX_KEY_MEDIA_VOLUME_DOWN = 355,
        SRCX_KEY_MEDIA_MUTE = 356
    };

    static XWindow initWindow(const int SCR_WIDTH, const int SCR_HEIGHT, const char* title, const int GLversionMajor, const int GLversionMinor)
    {
        glfwInit();
        if(!glfwInit()) {
            throw "SrcXEngine: error initializing GLFW.";
            glfwTerminate();
            //return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLversionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLversionMinor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
        if(!window) {
            throw "SrcXEngine: error creating GLFW window.";
            glfwTerminate();
            //return -1;
        }

        XWindow xWindow = {SCR_WIDTH, SCR_HEIGHT, title, window};

        return xWindow;
    }

    static void assignContextCurrent(XWindow window) {
        glfwMakeContextCurrent(window.window);
    }

    static bool windowShouldClose(XWindow window) {
        return window.shouldClose;
    }

    static void swapBuffers(XWindow window) {
        glfwSwapBuffers(window.window);
    }

    static void pollEvents() {
        glfwPollEvents();
    }

    static bool getKey(XWindow window, int keyCode) {
        return (glfwGetKey(window.window, keyCode) == GLFW_PRESS);
    }

    static void setWindowShouldClose(XWindow window, bool state) {
        window.shouldClose = state;
    }

    static XTexture loadTexture2D(std::string path, bool genMipmaps = true, bool flipVertically = true) {
        if(flipVertically)
            stbi_set_flip_vertically_on_load(true);
        else
            stbi_set_flip_vertically_on_load(false);

        XTexture texture;

        unsigned char *data = stbi_load(FileSystem::getPath(path).c_str(), texture.width, texture.height, texture.nrChannels, 0);
        unsigned int textureMap;

        glGenTextures(1, &textureMap);
    }
};

#endif //OPENGLTEST_SRCX_HPP
