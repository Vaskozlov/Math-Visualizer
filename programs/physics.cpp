#include <battery/embed.hpp>
#include <isl/shared_lib_loader.hpp>
#include <mv/application_2d.hpp>
#include <mv/gl/texture.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/gl/waterfall.hpp>
#include <mv/shader.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/io.hpp>

struct Cube
{
    glm::vec3 position;
    glm::vec4 color;
    double mass{};
    double velocity{};
    double acceleration{};
};

class PhysicsApplication : public mv::Application2D
{
private:
    std::array<char, 128> imguiWindowBufferTitle{};

    mv::Shader shader = mv::Shader{
        {b::embed<"resources/shaders/vertex.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader textureShader = mv::Shader{
        {b::embed<"resources/shaders/texture.vert">().str()},
        {b::embed<"resources/shaders/texture.frag">().str()},
    };

    static constexpr float cubeSize = 0.5F;

    mv::gl::Waterfall powerWaterfall{256, 256};
    mv::gl::Waterfall azimuthWaterfall{256, 256};

    std::string libraryPath;

    double pressTime = 0.0;
    ImFont *font;
    float fontScale = 0.33F;

    bool disableInput = false;

    mv::gl::VerticesContainer<glm::vec3> powerMapSize{
        {1.0F, 1.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F},
        {1.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
    };

    mv::gl::VerticesContainer<glm::vec3> azimuthMapSize{
        {1.0F, 1.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F},
        {1.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
    };

    isl::SharedLibLoader shaderLib;

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        ImGui::StyleColorsDark();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        powerMapSize.loadData();
        powerMapSize.vbo.bind();
        powerMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        powerMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        azimuthMapSize.loadData();
        azimuthMapSize.vbo.bind();
        azimuthMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        azimuthMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        camera.setPosition(glm::vec3(0.0F, 0.0F, 4.0F));
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBufferTitle.data(), imguiWindowBufferTitle.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBufferTitle.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 3.0F, "%.3f");
        ImGui::InputText("Lib path: ", &libraryPath);

        if (ImGui::Button("Load")) {
            shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reload")) {
            if (!shaderLib.hasLibrary()) {
                shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();
            } else {
                shaderLib.reload();
            }

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        textureShader.use();
        textureShader.setMat4("projection", getResultedViewMatrix());

        powerWaterfall.bind();
        powerWaterfall.reload();

        textureShader.setMat4("model", glm::mat4(1.0F));

        powerMapSize.vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(powerMapSize.vertices.size()));

        azimuthWaterfall.bind();
        azimuthWaterfall.reload();

        textureShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{1.2F, 0.0F, 0.0F}));

        azimuthMapSize.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(azimuthMapSize.vertices.size()));

        ImGui::PopFont();
        ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        if (!disableInput) {
            Application2D::processInput();
        }

        const auto left_alt_pressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
        const auto key_g_pressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

        if (left_alt_pressed && key_g_pressed) {
            const auto mode = glfwGetInputMode(window, GLFW_CURSOR);
            const double new_press_time = glfwGetTime();

            if (new_press_time - pressTime < key_press_delay) {
                return;
            }

            pressTime = new_press_time;
            firstMouse = true;
            isMouseShowed = mode == GLFW_CURSOR_DISABLED;

            glfwSetInputMode(
                window, GLFW_CURSOR, isMouseShowed ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
    }

    auto onMouseRelativeMovement(const double delta_x, const double delta_y) -> void override
    {
        const auto scale = camera.getZoom() / 9000.0F;

        camera.relativeMove(camera.getUp() * static_cast<float>(delta_y) * scale);
        camera.relativeMove(camera.getRight() * static_cast<float>(delta_x) * scale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = static_cast<double>(camera.getZoom()) / 90.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }
};


auto main() -> int
{
    PhysicsApplication application{1000, 800, "Fractal visualization", 2};
    application.run();

    return 0;
}
