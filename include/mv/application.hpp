#ifndef MV_APPLICATION_HPP
#define MV_APPLICATION_HPP

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <isl/isl.hpp>

#include <mv/camera.hpp>

namespace mv
{
    class Application
    {
    protected:
        Camera camera;
        GLFWwindow *window;
        float lastMouseX;
        float lastMouseY;
        float deltaTime = 0.0f;
        float lastFrameTime = 0.0f;
        float windowWidth;
        float windowHeight;
        float zNear = 0.1f;
        float zFar = 50.0f;
        bool firstMouse = true;
        bool isInFocus = true;
        bool isMouseShowed = false;

    public:
        virtual ~Application();

        Application(int width, int height, const std::string &title);

        [[nodiscard]] auto getCameraProjection() const -> glm::mat4
        {
            return glm::perspective(
                glm::radians(camera.getZoom()), windowWidth / windowHeight, zNear, zFar);
        }

        [[nodiscard]] auto getCameraView() const -> glm::mat4
        {
            return camera.getViewMatrix();
        }

        [[nodiscard]] auto getResultedViewMatrix() const -> glm::mat4 {
            return getCameraProjection() * getCameraView();
        }

        auto run() -> void;

        virtual auto init() -> void
        {}

        virtual auto update() -> void = 0;

        virtual auto onResize(int width, int height) -> void;

        virtual auto onMouseMovement(double xPosIn, double yPosIn) -> void;

        virtual auto onScroll(double xOffset, double yOffset) -> void;

        virtual auto processInput() -> void;

        virtual auto onLeaveOrEnter(bool entered) -> void;
    };
}// namespace mv

#endif /* MV_APPLICATION_HPP */
