#include "mv/application.hpp"
#include <battery/embed.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <mv/gl/gl_init.hpp>
#include <mv/glfw/callbacks.hpp>
#include <mv/glfw/glfw_init.hpp>
#include <thread>

namespace mv
{
    auto Application::loadFont(const float font_size) const -> ImFont *
    {
        void *font_data = const_cast<char *>(
            b::embed<"resources/fonts/JetBrainsMono-Medium.ttf">().data());// NOLINT

        const int font_data_size =
            static_cast<int>(b::embed<"resources/fonts/JetBrainsMono-Medium.ttf">().size());

        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;

        return imguiIO->Fonts->AddFontFromMemoryTTF(
            font_data, font_data_size, font_size, &config,
            imguiIO->Fonts->GetGlyphRangesCyrillic());
    }

    Application::Application(
        const int width, const int height, std::string window_title, const int multisampling_level)
      : title{std::move(window_title)}
      , windowWidth{static_cast<float>(width)}
      , windowHeight{static_cast<float>(height)}
    {
        glfw::init(3, 3);

        glfwSwapInterval(1);
        glfwWindowHint(GLFW_SAMPLES, multisampling_level);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (window == nullptr) {
            fmt::println(stderr, "Failed to create GLFW window.");
            glfwTerminate();
            std::terminate();
        }

        glfwMakeContextCurrent(window);
        glfw::setupCallbacksForApplication(this);

        gl::init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imguiIO = &ImGui::GetIO();

        imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    auto Application::onResize(const int width, const int height) -> void
    {
        windowWidth = static_cast<float>(width);
        windowHeight = static_cast<float>(height);
        glViewport(0, 0, width, height);
    }

    auto Application::onMouseMovement(const double x_pos_in, const double y_pos_in) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        if (firstMouse) {
            lastMouseX = x_pos_in;
            lastMouseY = y_pos_in;
            firstMouse = false;
        }

        const double x_offset = x_pos_in - lastMouseX;
        const double y_offset = lastMouseY - y_pos_in;

        lastMouseX = x_pos_in;
        lastMouseY = y_pos_in;

        onMouseRelativeMovement(x_offset, y_offset);
    }

    auto Application::onMouseRelativeMovement(const double delta_x, const double delta_y) -> void
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_TRUE) {
            camera.rotate(delta_x, delta_y);
            return;
        }

        camera.processMouseMovement(static_cast<float>(delta_x), static_cast<float>(delta_y));
    }

    auto Application::onScroll(const double /*x_offset*/, const double y_offset) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        camera.processMouseScroll(static_cast<float>(y_offset));
    }

    auto Application::run() -> void
    {
        using namespace std::chrono_literals;

        constexpr static auto delay_for_10_fps = 1.0 / 10.0;
        constexpr static auto delay_if_iconified = 100ms;

        init();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

        while (glfwWindowShouldClose(window) == GLFW_FALSE) {
            glfwWaitEventsTimeout(delay_for_10_fps);

            const auto current_time = static_cast<float>(glfwGetTime());
            deltaTime = current_time - lastFrameTime;
            lastFrameTime = current_time;

            if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
                std::this_thread::sleep_for(delay_if_iconified);
                continue;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            processInput();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            update();

            if (showImgui) {
                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            glfwSwapBuffers(window);
        }
    }

    auto Application::processInput() -> void
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    auto Application::onLeaveOrEnter(const bool entered) -> void
    {
        isInFocus = entered;
        firstMouse = true;
    }

    Application::~Application()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}// namespace mv
