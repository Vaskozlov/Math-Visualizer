#ifndef MV_APPLICATION_HPP
#define MV_APPLICATION_HPP

#include <GL/glew.h>
#include <imgui.h>

#include <GLFW/glfw3.h>
#include <isl/isl.hpp>

#include <mv/camera.hpp>

#include <battery/embed.hpp>

namespace mv
{
    class Application
    {
    protected:
        Camera camera;
        std::string title;
        glm::vec4 clearColor{0.0F, 0.0F, 0.0F, 1.0F};
        GLFWwindow *window;
        ImGuiIO *imguiIO;
        float lastMouseX = 0.0F;
        float lastMouseY = 0.0F;
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
        Application(int width, int height, std::string window_title, int multisampling_level = 4);

        virtual ~Application();

        [[nodiscard]] auto getCameraProjection() const -> glm::mat4
        {
            return glm::perspective(
                glm::radians(camera.getZoom()), windowWidth / windowHeight, zNear, zFar);
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

        auto run() -> void;

        virtual auto init() -> void
        {}

        virtual auto update() -> void = 0;

        virtual auto onResize(int width, int height) -> void;

        virtual auto onMouseMovement(double x_pos_in, double y_pos_in) -> void;

        virtual auto onMouseRelativeMovement(double delta_x, double delta_y) -> void;

        virtual auto onScroll(double x_offset, double y_offset) -> void;

        virtual auto processInput() -> void;

        virtual auto onLeaveOrEnter(bool entered) -> void;

        template<b::embed_string_literal fontPath>
        [[nodiscard]] auto loadFont(const float font_size = 45.0F) const -> ImFont *
        {
            void *font_data = const_cast<char *>(b::embed<fontPath>().data());// NOLINT
            const int font_data_size = b::embed<fontPath>().size();

            ImFontConfig config;
            config.FontDataOwnedByAtlas = false;

            return imguiIO->Fonts->AddFontFromMemoryTTF(
                font_data, font_data_size, font_size, &config,
                imguiIO->Fonts->GetGlyphRangesCyrillic());
        }
    };
}// namespace mv

#endif /* MV_APPLICATION_HPP */
