#define TRACY_ENABLE 1

#include <battery/embed.hpp>
#include <imgui.h>
#include <mv/application_2d.hpp>
#include <mv/gl/instancing.hpp>
#include <mv/gl/vertices_binder.hpp>
#include <mv/shader.hpp>

enum class FractalType : std::size_t
{
    MANDELBROT = 0,
    JULIA = 1,
    NEWTON = 2,
    NEWTON_CLASSIC = 3,
    NEWTON_SIN = 4,
};

class FractalApplication final : public mv::Application2D
{
private:
    std::array<char, 128> imguiWindowBuffer{};

    mv::Shader mandelbrotFractalShader = mv::Shader{
        {b::embed<"resources/shaders/fractal/default.vert">().str()},
        {
            b::embed<"resources/shaders/fractal/mandelbrot.frag">().str(),
            b::embed<"resources/shaders/fractal/common.glsl">().str(),
        },
    };

    mv::Shader juliaFractalShader = mv::Shader{
        {b::embed<"resources/shaders/fractal/with_offset.vert">().str()},
        {
            b::embed<"resources/shaders/fractal/julia.frag">().str(),
            b::embed<"resources/shaders/fractal/common.glsl">().str(),
        },
    };

    mv::Shader newtonFractalShader = mv::Shader{
        {b::embed<"resources/shaders/fractal/default.vert">().str()},
        {
            b::embed<"resources/shaders/fractal/newton.frag">().str(),
            b::embed<"resources/shaders/fractal/common.glsl">().str(),
        },
    };

    mv::Shader newtonClassicFractalShader = mv::Shader{
        {b::embed<"resources/shaders/fractal/default.vert">().str()},
        {
            b::embed<"resources/shaders/fractal/newton_classic.frag">().str(),
            b::embed<"resources/shaders/fractal/common.glsl">().str(),
        },
    };

    mv::Shader newtonSinFractalShader = mv::Shader{
        {b::embed<"resources/shaders/fractal/default.vert">().str()},
        {
            b::embed<"resources/shaders/fractal/newton_sin.frag">().str(),
            b::embed<"resources/shaders/fractal/common.glsl">().str(),
        },
    };

    mv::gl::VerticesContainer<glm::vec3> mandelbrotVertices{
        {2.0F, 2.0F, 0.0F}, {2.0F, -2.0F, 0.0F},  {-2.0F, -2.0F, 0.0F},
        {2.0F, 2.0F, 0.0F}, {-2.0F, -2.0F, 0.0F}, {-2.0F, 2.0F, 0.0F},
    };

    mv::gl::VerticesContainer<glm::vec3> juliaVertices{
        {2.0F, 2.0F, 0.0F}, {2.0F, -2.0F, 0.0F},  {-2.0F, -2.0F, 0.0F},
        {2.0F, 2.0F, 0.0F}, {-2.0F, -2.0F, 0.0F}, {-2.0F, 2.0F, 0.0F},
    };

    mv::gl::VerticesContainer<glm::vec3> newtonVertices{
        {50.0F, 50.0F, 0.0F}, {50.0F, -50.0F, 0.0F},  {-50.0F, -50.0F, 0.0F},
        {50.0F, 50.0F, 0.0F}, {-50.0F, -50.0F, 0.0F}, {-50.0F, 50.0F, 0.0F},
    };

    ImFont *font;
    float fontScale = 0.5F;

    double pressTime = 0.0;
    glm::vec2 fractalC{-0.8F, 0.156};

    int newtonIterations = 45;
    int newtonClassicIterations = 45;
    int newtonSinIterations = 45;
    int juliaIterations = 1000;
    int mandelbrotIterations = 1000;

    std::size_t fractalIndex = 0;

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        mandelbrotVertices.loadData();
        juliaVertices.loadData();
        newtonVertices.loadData();

        mandelbrotVertices.vbo.bind();
        mandelbrotVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        juliaVertices.vbo.bind();
        juliaVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        newtonVertices.vbo.bind();
        newtonVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        camera.movementSpeed = 0.5F;

        ImGui::StyleColorsDark();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        updateShaderUniform();
    }

    auto juliaFractal() -> void
    {
        if (ImGui::SliderInt("Iterataions", &juliaIterations, 5, 4000)) {
            updateShaderUniform();
        }

        if (ImGui::SliderFloat("Re(z)", &fractalC.x, -1.0f, 1.0f)) {
            updateJuliaShaderUniform();
        }

        if (ImGui::SliderFloat("Im(z)", &fractalC.y, -1.0f, 1.0f)) {
            updateJuliaShaderUniform();
        }

        juliaFractalShader.use();
        juliaFractalShader.setMat4("projection", getResultedViewMatrix());
        juliaVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, juliaVertices.vertices.size());
    }

    auto mandelbrotFractal() -> void
    {
        if (ImGui::SliderInt("Iterataions", &mandelbrotIterations, 5, 4000)) {
            updateShaderUniform();
        }

        mandelbrotFractalShader.use();
        mandelbrotFractalShader.setMat4("projection", getResultedViewMatrix());
        mandelbrotVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, mandelbrotVertices.vertices.size());
    }

    auto newtonFractal() -> void
    {
        if (ImGui::SliderInt("Iterataions", &newtonIterations, 5, 200)) {
            updateShaderUniform();
        }

        newtonFractalShader.use();
        newtonFractalShader.setMat4("projection", getResultedViewMatrix());
        newtonVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, newtonVertices.vertices.size());
    }

    auto newtonClassicFractal() -> void
    {
        if (ImGui::SliderInt("Iterataions", &newtonClassicIterations, 5, 200)) {
            updateShaderUniform();
        }

        newtonClassicFractalShader.use();
        newtonClassicFractalShader.setMat4("projection", getResultedViewMatrix());
        newtonVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, newtonVertices.vertices.size());
    }

    auto newtonSinFractal() -> void
    {
        if (ImGui::SliderInt("Iterataions", &newtonSinIterations, 5, 200)) {
            updateShaderUniform();
        }

        newtonSinFractalShader.use();
        newtonSinFractalShader.setMat4("projection", getResultedViewMatrix());
        newtonVertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, newtonVertices.vertices.size());
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        switch (static_cast<FractalType>(fractalIndex)) {
        case FractalType::MANDELBROT:
            mandelbrotFractal();
            break;

        case FractalType::JULIA:
            juliaFractal();
            break;

        case FractalType::NEWTON:
            newtonFractal();
            break;

        case FractalType::NEWTON_CLASSIC:
            newtonClassicFractal();
            break;

        case FractalType::NEWTON_SIN:
            newtonSinFractal();
            break;

        default:
            isl::unreachable();
        }

        if (ImGui::Button("Center camera")) {
            camera.position = glm::vec3(0.0F, 0.0F, 2.0F);
        }

        ImGui::SameLine();

        if (ImGui::Button("Change fractal")) {
            fractalIndex = (fractalIndex + 1) % 5;
            updateShaderUniform();
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        ImGui::PopFont();
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
        const auto scale = camera.zoom / 9000.0F;

        camera.position += camera.up * static_cast<float>(delta_y) * scale;
        camera.position += camera.right * static_cast<float>(delta_x) * scale;
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = static_cast<double>(camera.zoom) / 90.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }

private:
    auto updateShaderUniform() const -> void
    {
        switch (static_cast<FractalType>(fractalIndex)) {
        case FractalType::MANDELBROT:
            mandelbrotFractalShader.use();
            mandelbrotFractalShader.setInt("iterations", mandelbrotIterations);
            break;

        case FractalType::JULIA:
            updateJuliaShaderUniform();
            break;

        case FractalType::NEWTON:
            newtonFractalShader.use();
            newtonFractalShader.setInt("iterations", newtonIterations);
            break;

        case FractalType::NEWTON_CLASSIC:
            newtonClassicFractalShader.use();
            newtonClassicFractalShader.setInt("iterations", newtonClassicIterations);
            break;

        case FractalType::NEWTON_SIN:
            newtonSinFractalShader.use();
            newtonSinFractalShader.setInt("iterations", newtonSinIterations);
            break;

        default:
            isl::unreachable();
        }
    }

    auto updateJuliaShaderUniform() const -> void
    {
        juliaFractalShader.use();
        juliaFractalShader.setInt("iterations", juliaIterations);
        juliaFractalShader.setVec2("fC", fractalC);
    }
};

auto main() -> int
{
    FractalApplication application{1000, 800, "Fractal visualization", 2};
    application.run();
    return 0;
}
