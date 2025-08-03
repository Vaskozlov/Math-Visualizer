#ifndef MV_APPLICATION_HPP
#define MV_APPLICATION_HPP

#include <cstdlib>
#include <deque>
#include <functional>
#include <imgui.h>
#include <isl/io.hpp>
#include <isl/isl.hpp>
#include <isl/thread/async_task.hpp>
#include <isl/thread/pool.hpp>
#include <mutex>
#include <mv/camera.hpp>
#include <mv/gl/gl_init.hpp>
#include <mv/shader.hpp>

//

#include <GLFW/glfw3.h>

namespace mv
{
    [[nodiscard]] auto findOutResourcesPath(const int argc, const char *argv[]) -> std::string;

    class Application
    {
    protected:
        friend auto invokeLoop() -> void;

        isl::thread::Pool pool{0, false};
        Camera camera;
        std::string title;
        glm::vec4 clearColor{0.0F, 0.0F, 0.0F, 1.0F};
        GLFWwindow *window;
        ImGuiIO *imguiIO{};
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;
        double cursorModePressTime = 0.0;
        float deltaTime = 0.0F;
        float lastFrameTime = 0.0F;
        float windowWidth;
        float windowHeight;
        float zNear = 0.1F;
        float zFar = 50.0F;
        bool firstMouse = true;
        bool isInFocus = true;
        bool isMouseShowed = false;
        bool showImgui = true;

        std::deque<std::function<void()>> onMainThreadExecutionQueue;
        std::mutex onMainThreadExecutionQueueMutex;

        std::filesystem::path resourcesPath;

    public:
        [[nodiscard]] auto getColorShader() const -> Shader;

        [[nodiscard]] auto getTexture3DLinearShader() const -> Shader;

        [[nodiscard]] auto getShaderWithPositioning() const -> Shader;

        [[nodiscard]] auto getHsvShaderWithModel() const -> Shader;

        [[nodiscard]] auto getLinearShaderWithModel() const -> Shader;

        Application(
            std::filesystem::path programs_path, int width, int height, std::string window_title,
            int multisampling_level = 4);

        virtual ~Application();

        [[nodiscard]] auto getResourceAsString(const std::string_view name) const -> std::string
        {
            return isl::io::read(resourcesPath / name);
        }

        [[nodiscard]] auto getResourceAsRaw(const std::string_view name) const
            -> std::pair<std::size_t, void *>
        {
            auto result = isl::io::read(resourcesPath / name);
            auto buffer = std::make_unique_for_overwrite<char[]>(result.size());

            std::copy_n(result.data(), result.size(), buffer.get());

            return std::make_pair(result.size(), static_cast<void *>(buffer.release()));
        }

        [[nodiscard]] virtual auto getCameraProjection() const -> glm::mat4
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

        template <typename... Args>
        static auto imguiText(fmt::format_string<Args...> fmt, Args &&...args) -> void
        {
            const auto formnatted = fmt::format(fmt, std::forward<Args>(args)...);

            ImGui::TextUnformatted(formnatted.c_str(), formnatted.c_str() + formnatted.size());
        }

        auto setClearColor(const glm::vec4 &clear_color) -> void
        {
            clearColor = clear_color;
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        }

        auto submit(isl::Task<> task) -> isl::AsyncTask<void>;

        auto run() -> void;

        virtual auto drawGUI() -> void
        {}

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

        virtual auto onDrop(const std::vector<std::filesystem::path> &paths) -> void;

        [[nodiscard]] auto loadFont(float font_size = 45.0F) const -> ImFont *;

    private:
        auto loop() -> void;
    };
} // namespace mv

#endif /* MV_APPLICATION_HPP */
