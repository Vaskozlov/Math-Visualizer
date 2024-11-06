#ifndef MV_GLFW_CALLBACKS_HPP
#define MV_GLFW_CALLBACKS_HPP

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace mv::glfw
{
    auto frameBufferSizeCallback(GLFWwindow *window, int width, int height);

    auto mouseCallback(GLFWwindow *window, double x, double y);

    auto scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

    auto cursorEnterLeaveCallback(GLFWwindow *window, int entered);

    auto setupCallbacksForApplication(Application *application) -> void;
}// namespace mv::glfw

#endif /* MV_GLFW_CALLBACKS_HPP */
