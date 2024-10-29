#include "mv/application.hpp"
#include <unordered_map>

namespace mv
{
    static auto frameBufferSizeCallback(GLFWwindow *window, const int width, const int height)
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onResize(width, height);
    }

    static auto mouseCallback(GLFWwindow *window, const double x, const double y)
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onMouseMovement(x, y);
    }

    static auto scrollCallback(GLFWwindow *window, const double xOffset, const double yOffset)
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onScroll(xOffset, yOffset);
    }

    static auto cursorEnterLeaveCallback(GLFWwindow *window, const int entered)
    {
        auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->onLeaveOrEnter(entered == GLFW_TRUE);
    }

    Application::Application(const int width, const int height, const std::string &title)
      : lastMouseX{static_cast<float>(width) / 2.0F}
      , lastMouseY{static_cast<float>(height) / 2.0F}
      , windowWidth{static_cast<float>(width)}
      , windowHeight{static_cast<float>(height)}
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (window == nullptr) {
            fmt::println(stderr, "Failed to create GLFW window.");
            glfwTerminate();
            std::terminate();
        }

        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetCursorEnterCallback(window, cursorEnterLeaveCallback);

        glewInit();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    auto Application::onResize(const int width, const int height) -> void
    {
        windowWidth = static_cast<float>(width);
        windowHeight = static_cast<float>(height);
        glViewport(0, 0, width, height);
    }

    auto Application::onMouseMovement(const double xPosIn, const double yPosIn) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        const auto x_pos = static_cast<float>(xPosIn);
        const auto y_pos = static_cast<float>(yPosIn);

        if (firstMouse) {
            lastMouseX = x_pos;
            lastMouseY = y_pos;
            firstMouse = false;
        }

        const float x_offset = x_pos - lastMouseX;
        const float y_offset = lastMouseY - y_pos;

        lastMouseX = x_pos;
        lastMouseY = y_pos;

        camera.processMouseMovement(x_offset, y_offset);
    }

    auto Application::onScroll(const double xOffset, const double yOffset) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        camera.processMouseScroll(static_cast<float>(yOffset));
    }

    auto Application::run() -> void
    {
        init();

        while (!glfwWindowShouldClose(window)) {
            const auto current_time = static_cast<float>(glfwGetTime());
            deltaTime = current_time - lastFrameTime;
            lastFrameTime = current_time;

            processInput();
            update();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    auto Application::processInput() -> void
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(CameraMovement::LEFT, deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
        }
    }

    auto Application::onLeaveOrEnter(const bool entered) -> void
    {
        isInFocus = entered;
        firstMouse = true;
        fmt::println("{}", isInFocus);
    }
}// namespace mv