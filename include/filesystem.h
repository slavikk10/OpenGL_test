//
// Created by Vyacheslav on 01.04.2025.
//

#ifndef OPENGLTEST_FILESYSTEM_H
#define OPENGLTEST_FILESYSTEM_H

#include <string>
#include <iostream>

class FileSystem {
public:
    static std::string getPath(const std::string& path) {
        return "/Users/vyacheslav/OpenGL_C++_test_scene/" + path;
    }
};

#endif //OPENGLTEST_FILESYSTEM_H
