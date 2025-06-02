#include <SrcXEngine/srcx.hpp>
#include <iostream>
#include <SrcXEngine/shader.h>

void processInput(XWindow window);

int main() {
    XWindow window = SrcX::initWindow(1920, 1040, "Src X window!", 3, 3);
    SrcX::assignContextCurrent(window);



    while(!SrcX::windowShouldClose(window)) {
        processInput(window);

        // rendering, etc.

        SrcX::swapBuffers(window);
        SrcX::pollEvents();
    }
}

void processInput(XWindow window) {
    if(SrcX::getKey(window, SrcX::SRCX_KEY_ESCAPE))
        SrcX::setWindowShouldClose(window, true);
    std::cout << window.shouldClose << std::endl;
}
