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
    mv::gl::shape::Plot2D dftGraph;

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> sphereInstances;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1000);
    std::valarray<float> functionY;

    float sphereRadius = 0.1F;

    std::size_t rightRootIterations = 0;
    float rightRoot = 0.0F;
    float rightRootDelta = 0.0F;

    std::size_t leftRootIterations = 0;
    float leftRoot = 0.0F;
    float leftRootDelta = 0.0F;

    std::size_t middleRootIterations = 0;
    float middleRoot = 0.0F;
    float middleRootDelta = 0.0F;

    std::list<mv::gl::shape::Plot2D> rightRootsLines;

public:
    using Application2D::Application2D;

    static auto function(const float x) -> float
    {
        return x * x * x - 3.125F * x * x - 3.5F * x + 2.458F;
    }

    static auto derivation(const float x) -> float
    {
        return 3.0F * x * x - 6.25F * x - 3.5F;
    }

    static auto newtonIterationX(const float x0) -> float
    {
        return x0 - function(x0) / derivation(x0);
    }

    static auto phi(const float x) -> float
    {
        return (x * x * x - 3.125 * x * x + 2.458F) / 3.5F;
    }

    static auto phiDerivation(const float x) -> float
    {
        return (3.0F * x * x - 6.25F) / 3.5F;
    }

    static auto simpleIteration(const float x) -> float
    {
        return phi(x);
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

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

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        upperFunctionGraph.loadData();
        upperFunctionGraph.vbo.bind();
        upperFunctionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        dftGraph.loadData();
        dftGraph.vbo.bind();
        dftGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstances.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        ImGui::StyleColorsLight();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);

        functionY = std::valarray<float>(functionX.size());

        for (std::size_t i = 0; i < functionX.size(); ++i) {
            const auto x = functionX[i];
            functionY[i] = x * x * x - 3.125F * x * x - 3.5F * x + 2.458F;
        }

        upperFunctionGraph.fill(functionX, functionY, 0.07F);
        upperFunctionGraph.loadData();

        float xi = 6.0F;

        while (std::abs(function(xi)) > epsilon && rightRootIterations < maxIterations) {
            sphereInstances.models.emplace_back(
                mv::Color::LIGHT_GREEN,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            xi,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            std::valarray<float> newX(3);
            std::valarray<float> newY(3);

            const auto new_xi = newtonIterationX(xi);

            newX[0] = new_xi;
            newX[1] = xi;

            newY[0] = 0.0F;
            newY[1] = function(xi);

            xi = new_xi;
            rightRootIterations++;

            auto &line = rightRootsLines.emplace_back();
            line.fill(newX, newY, 0.05F);
            line.loadData();
            line.vbo.bind();
            line.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
        }

        rightRoot = xi;

        xi = 0.0F;
        while (std::abs(function(xi)) > epsilon && middleRootIterations < maxIterations) {
            xi = simpleIteration(xi);

            sphereInstances.models.emplace_back(
                mv::Color::NAVY,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            xi,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            ++middleRootIterations;
        }

        middleRoot = xi;

        auto a = -10.0F;
        auto b = middleRoot - 1e-2F;
        xi = (a + b) * 0.5F;

        while (std::abs(function(xi)) > epsilon && leftRootIterations < maxIterations) {
            xi = (a + b) * 0.5F;

            if (function(a) * function(xi) > 0) {
                a = xi;
            } else {
                b = xi;
            }

            ++leftRootIterations;

            sphereInstances.models.emplace_back(
                mv::Color::FOREST,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            xi,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));
        }

        leftRoot = (a + b) * 0.5F;
        sphereInstances.loadData();

        leftRootDelta = std::abs(function(leftRoot));
        middleRoot = std::abs(function(middleRoot));
        rightRootDelta = std::abs(function(rightRoot));
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
            "Left root: %f, iterations: %zu, delta: %f\n"
            "Middle root: %f, iterations: %zu, delta: %f\n"
            "Right root: %f, iterations: %zu, delta: %f\n",
            leftRoot, leftRootIterations, leftRootDelta, middleRoot, middleRootIterations,
            middleRootDelta, rightRoot, rightRootIterations, rightRootDelta);

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);
        upperFunctionGraph.draw();

        colorShader.setVec4("elementColor", mv::Color::RED);
        for (auto const &right_lines : rightRootsLines) {
            right_lines.draw();
        }

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
