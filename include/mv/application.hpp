#ifndef MV_APPLICATION_HPP
#define MV_APPLICATION_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <deque>
#include <functional>
#include <imgui.h>
#include <isl/isl.hpp>
#include <mutex>
#include <mv/camera.hpp>

namespace mv
{
    class Application
    {
    protected:
        Camera camera;
        std::string title;
        glm::vec4 clearColor{0.0F, 0.0F, 0.0F, 1.0F};
        GLFWwindow *window;
        ImGuiIO *imguiIO{};
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        float deltaTime = 0.0f;
        float lastFrameTime = 0.0f;
        float windowWidth;
        float windowHeight;
        float zNear = 0.1f;
        float zFar = 50.0f;
        bool firstMouse = true;
        bool isInFocus = true;
        bool isMouseShowed = false;
        bool showImgui = true;

        std::deque<std::function<void()>> onMainThreadExecutionQueue;
        std::mutex onMainThreadExecutionQueueMutex;

    public:
        Application(int width, int height, std::string window_title, int multisampling_level = 4);

        virtual ~Application();

        [[nodiscard]] auto getCameraProjection() const -> glm::mat4
        {
            return glm::perspective(
                glm::radians(camera.getZoom()), windowWidth / windowHeight, zNear, zFar);
        }

        [[nodiscard]] auto getCameraProjection(const float zoomX, const float zoomY) const
            -> glm::mat4
        {
            return glm::ortho(
                -1.0F / zoomX, 1.0F / zoomX, -1.0F / zoomY, 1.0F / zoomY, zNear, zFar);
        }

        [[nodiscard]] auto getCameraView() const -> glm::mat4
        {
            return camera.getViewMatrix();
        }

        [[nodiscard]] auto getResultedViewMatrix() const -> glm::mat4
        {
            return getCameraProjection() * getCameraView();
        }

        [[nodiscard]] auto getWindow() const -> GLFWwindow *
        {
            return window;
        }

        [[nodiscard]] auto getClearColor() const -> const glm::vec4 &
        {
            return clearColor;
        }

        auto setClearColor(const glm::vec4 &clear_color) -> void
        {
            clearColor = clear_color;
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        }

        auto submit(const std::function<void()> &func) -> void;

        auto submit(const std::function<void(Application &)> &func) -> void;

        auto submit(const std::function<void(const Application &)> &func) -> void;

        auto run() -> void;

        virtual auto init() -> void
        {}

        virtual auto update() -> void;

        virtual auto onResize(int width, int height) -> void;

        virtual auto onMouseMovement(double x_pos_in, double y_pos_in) -> void;

        virtual auto onMouseRelativeMovement(double delta_x, double delta_y) -> void;

        virtual auto onScroll(double x_offset, double y_offset) -> void;

        virtual auto processInput() -> void;

        virtual auto onLeaveOrEnter(bool entered) -> void;

        virtual auto onMouseClick(int button, int action, int mods) -> void;

        [[nodiscard]] auto loadFont(float font_size = 45.0F) const -> ImFont *;
    };
} // namespace mv

#endif /* MV_APPLICATION_HPP */
