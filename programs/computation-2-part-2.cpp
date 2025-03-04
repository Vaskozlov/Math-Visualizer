#include <battery/embed.hpp>
#include <complex>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/linalg/linspace.hpp>
#include <list>
#include <mv/application_2d.hpp>
#include <mv/application_3d.hpp>
#include <mv/color.hpp>
#include <mv/gl/axes_2d.hpp>
#include <mv/gl/instances_holder.hpp>
#include <mv/gl/shape/plot_2d.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/shader.hpp>
#include <numbers>
#include <valarray>

class Computation2 final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static float epsilon = 1e-4F;
    constexpr static std::size_t maxIterations = 1000;

    constexpr static glm::vec3 defaultCameraPosition{0.0F, 0.0F, 10.0F};

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader colorShader = mv::Shader{
        {b::embed<"resources/shaders/colored_shader.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader shaderWithPositioning = mv::Shader{
        {b::embed<"resources/shaders/static_instance.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::gl::shape::Axes2D plot{12, 0.009F};

    mv::gl::shape::Plot2D upperFunctionGraph;
    mv::gl::shape::Plot2D lowerFunctionGraph;

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> sphereInstances;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionXUpper = isl::linalg::linspace<float>(-10, 10, 1000);
    std::valarray<float> functionYUpper;
    std::valarray<float> functionYLower = functionXUpper;
    std::valarray<float> functionXLower;

    float sphereRadius = 0.1F;

    float resultX = 4.0F;
    float resultY = 4.0F;
    std::size_t iteration = 0;

    float inputX = 4.0F;
    float inputY = 4.0F;

    glm::vec2 errorVector{};

public:
    using Application2D::Application2D;

    static auto upperFunction(const float x) -> float
    {
        return std::sin(x + 0.5F) - 1.0F;
    }

    static auto lowerFunction(const float y) -> float
    {
        return -std::cos(y - 2.0F);
    }

    auto compute(const float start_x, const float start_y) -> void
    {
        resultX = start_x;
        resultY = start_y;

        bool shouldStop = false;
        sphereInstances.models.clear();

        sphereInstances.models.emplace_back(
            mv::Color::PINK,
            glm::scale(
                glm::translate(
                    glm::mat4(1.0F),
                    {
                        0.0F,
                        0.0F,
                        0.01F,
                    }),
                glm::vec3{sphereRadius}));

        sphereInstances.models.emplace_back(
            mv::Color::CYAN,
            glm::scale(
                glm::translate(
                    glm::mat4(1.0F),
                    {
                        resultX,
                        resultY,
                        0.01F,
                    }),
                glm::vec3{sphereRadius}));

        while (iteration < maxIterations && !shouldStop) {
            const auto new_x = -std::cos(resultY - 2.0F);
            const auto new_y = std::sin(resultX + 0.5F) - 1.0F;

            sphereInstances.models.emplace_back(
                mv::Color::NAVY,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            new_x,
                            new_y,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            errorVector.x = std::abs(new_x - resultX);
            errorVector.y = std::abs(new_y - resultY);

            if (errorVector.x < epsilon && errorVector.y < epsilon) {
                shouldStop = true;
            }

            resultX = new_x;
            resultY = new_y;
            ++iteration;
        }

        sphereInstances.loadData();
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        upperFunctionGraph.loadData();
        upperFunctionGraph.vbo.bind();
        upperFunctionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        lowerFunctionGraph.loadData();
        lowerFunctionGraph.vbo.bind();
        lowerFunctionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstances.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        ImGui::StyleColorsLight();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);

        functionYUpper = std::valarray<float>(functionXUpper.size());

        for (std::size_t i = 0; i < functionXUpper.size(); ++i) {
            const auto x = functionXUpper[i];
            functionYUpper[i] = upperFunction(x);
        }

        upperFunctionGraph.fill(functionXUpper, functionYUpper, 0.07F);
        upperFunctionGraph.loadData();

        functionXLower = std::valarray<float>(functionYLower.size());

        for (std::size_t i = 0; i < functionYLower.size(); ++i) {
            const float y = functionYLower[i];
            functionXLower[i] = lowerFunction(y);
        }

        lowerFunctionGraph.fill(functionXLower, functionYLower, 0.07F);
        lowerFunctionGraph.loadData();
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::Text(
            "Result: (%f, %f), iterations: %zu, iteration dif: (%f, %f)", resultX, resultY,
            iteration, errorVector.x, errorVector.y);

        ImGui::SliderFloat("Start x: %f", &inputX, -10.0F, 10.0F);
        ImGui::SliderFloat("Start y: %f", &inputY, -10.0F, 10.0F);

        if (ImGui::Button("Compute")) {
            compute(inputX, inputY);
        }

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);
        upperFunctionGraph.draw();
        lowerFunctionGraph.draw();

        colorShader.setVec4("elementColor", mv::Color::RED);

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), sphereInstances.models.size());

        if (ImGui::Button("Center camera")) {
            camera.setPosition(defaultCameraPosition);
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        ImGui::PopFont();
        ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        Application2D::processInput();

        const auto left_shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        const auto key_g_pressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

        if (left_shift_pressed && key_g_pressed) {
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
        const auto scale = static_cast<double>(camera.getZoom()) / 180.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }
};

auto main() -> int
{
    Computation2 application{1000, 800, "Computation2", 2};
    application.run();
    return 0;
}
