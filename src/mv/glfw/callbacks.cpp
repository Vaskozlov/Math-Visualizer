#include <mv/application.hpp>
#include <mv/glfw/callbacks.hpp>

namespace mv::glfw
{
    auto frameBufferSizeCallback(GLFWwindow *window, const int width, const int height) -> void
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onResize(width, height);
    }

    auto mouseCallback(GLFWwindow *window, const double x, const double y) -> void
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onMouseMovement(x, y);
    }

    auto scrollCallback(GLFWwindow *window, const double xOffset, const double yOffset) -> void
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onScroll(xOffset, yOffset);
    }

    auto cursorEnterLeaveCallback(GLFWwindow *window, const int entered) -> void
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onLeaveOrEnter(entered == GLFW_TRUE);
    }

    auto mouseClickCallback(GLFWwindow *window, const int button, const int action, const int mods)
        -> void
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onMouseClick(button, action, mods);
    }

    auto setupCallbacksForApplication(Application *application) -> void
    {
        GLFWwindow *window = application->getWindow();

        glfwSetMouseButtonCallback(window, mouseClickCallback);
        glfwSetWindowUserPointer(window, application);
        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetCursorEnterCallback(window, cursorEnterLeaveCallback);
    }
}// namespace mv::glfw
