#include <battery/embed.hpp>
#include <imgui.h>
#include <mv/application_2d.hpp>
#include <mv/gl/instancing.hpp>
#include <mv/gl/vertices_binder.hpp>
#include <mv/shader.hpp>


class MandelbrotFractalApplication final : public mv::Application2D
{
private:
    std::array<char, 128> imguiWindowBuffer{};

    mv::Shader mandelbrotFractalShader = mv::Shader{
        b::embed<"resources/shaders/mandelbrot_fractal.vert">().str(),
        b::embed<"resources/shaders/mandelbrot_fractal.frag">().str(),
    };

    mv::Shader juliaFractalShader = mv::Shader{
        b::embed<"resources/shaders/julia_fractal.vert">().str(),
        b::embed<"resources/shaders/julia_fractal.frag">().str(),
    };

    mv::gl::VerticesContainer<glm::vec3> mandelbrotVertices{
        {2.0F, 2.0F, 0.0F}, {2.0F, -2.0F, 0.0F},  {-2.0F, -2.0F, 0.0F},
        {2.0F, 2.0F, 0.0F}, {-2.0F, -2.0F, 0.0F}, {-2.0F, 2.0F, 0.0F},
    };

    mv::gl::VerticesContainer<glm::vec3> juliaVertices{
        {2.0F, 2.0F, 0.0F}, {2.0F, -2.0F, 0.0F},  {-2.0F, -2.0F, 0.0F},
        {2.0F, 2.0F, 0.0F}, {-2.0F, -2.0F, 0.0F}, {-2.0F, 2.0F, 0.0F},
    };

    double pressTime = 0.0;
    glm::vec2 fractalC{-0.8F, 0.156};

    int iterations = 1000;
    bool useJuliaShader = false;

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        mandelbrotVertices.loadData();
        juliaVertices.loadData();

        mandelbrotVertices.vbo.bind();
        mandelbrotVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        juliaVertices.vbo.bind();
        juliaVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        camera.movementSpeed = 0.5F;
        ImGui::StyleColorsDark();
    }

    auto juliaFractal() -> void
    {
        juliaFractalShader.use();

        if (ImGui::SliderFloat("Re(z)", &fractalC.x, -1.0f, 1.0f)) {
            updateJuliaShaderUniform();
        }

        if (ImGui::SliderFloat("Im(z)", &fractalC.y, -1.0f, 1.0f)) {
            updateJuliaShaderUniform();
        }

        juliaFractalShader.setMat4("projection", getResultedViewMatrix());
        juliaVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, juliaVertices.vertices.size());
    }

    auto mandelbrotFractal() const -> void
    {
        mandelbrotFractalShader.use();

        mandelbrotFractalShader.setInt("iterations", iterations);
        mandelbrotFractalShader.setMat4("projection", getResultedViewMatrix());
        mandelbrotVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, mandelbrotVertices.vertices.size());
    }

    auto update() -> void override
    {
        fmt::format_to(
            imguiWindowBuffer.data(),
            "Settings. FPS: {:#.4}###SettingWindowTitle",
            ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());

        if (ImGui::SliderInt("Iterataions", &iterations, 5, 4000)) {
            updateShaderUniform();
        }

        if (useJuliaShader) {
            juliaFractal();
        } else {
            mandelbrotFractal();
        }

        if (ImGui::Button("Center camera")) {
            camera.position = glm::vec3(0.0F, 0.0F, 2.0F);
        }

        ImGui::SameLine();

        if (ImGui::Button("Change fractal")) {
            useJuliaShader = !useJuliaShader;
            updateShaderUniform();
        }

        ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        Application2D::processInput();

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
        const auto scale = camera.zoom / 9000.0;

        camera.position += camera.up * static_cast<float>(delta_y * scale);
        camera.position += camera.right * static_cast<float>(delta_x * scale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = camera.zoom / 90.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }

private:
    auto updateShaderUniform() const -> void
    {
        if (useJuliaShader) {
            updateJuliaShaderUniform();
            return;
        }

        updateMandelbrotShaderUniform();
    }

    auto updateJuliaShaderUniform() const -> void
    {
        juliaFractalShader.use();
        juliaFractalShader.setInt("iterations", iterations);
        juliaFractalShader.setVec2("fC", fractalC);
    }

    auto updateMandelbrotShaderUniform() const -> void
    {
        mandelbrotFractalShader.use();
        mandelbrotFractalShader.setInt("iterations", iterations);
    }
};

auto main() -> int
{
    MandelbrotFractalApplication application{1000, 800, "Fractal visualization"};
    application.run();
    return 0;
}
