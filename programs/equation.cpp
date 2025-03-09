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
#include <mvl/mvl.hpp>
#include <numbers>
#include <valarray>

class Computation2 final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static float epsilon = 1e-5F;
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

    mv::gl::shape::Plot2D functionGraph;
    mv::gl::shape::Plot2D dftGraph;

    mv::gl::shape::Prism prism{0.03F, 20.0F, 4};

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> sphereInstances;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1000);
    std::valarray<float> functionY = functionX;

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

    std::string input = "x^3-3.125*x^2-3.5*x+2.458";

    float leftBorder = -5.0F;
    float rightBorder = 5.0F;

    ccl::parser::ast::SharedNode<mvl::ast::MathNode> root;

public:
    using Application2D::Application2D;

    auto constructPlot() -> void
    {
        for (std::size_t i = 0; i != functionX.size(); ++i) {
            functionY[i] = root->compute(functionX[i], 0.0);
        }

        functionGraph.fill(functionX, functionY);
        functionGraph.loadData();

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

        chordMethod();
        secantMethod();
        iterationsMethod();
        sphereInstances.loadData();
    }

    auto chordMethod() -> void
    {
        auto left_border = leftBorder;
        auto right_border = rightBorder;

        auto f = [this](float x) {
            return root->compute(x, 0.0);
        };

        float xi = left_border - (right_border - left_border) / (f(right_border) - f(left_border)) *
                                     f(left_border);

        std::size_t it = 0;

        while (std::abs(f(xi)) > epsilon && it < maxIterations) {
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

            auto left = f(leftBorder);
            auto center = f(xi);

            if (left * center < 0) {
                right_border = xi;
            } else {
                left_border = xi;
            }

            xi = left_border -
                 (right_border - left_border) / (f(right_border) - f(left_border)) * f(left_border);

            ++it;
        }

        fmt::println("{} {} {}", it, xi, f(xi));
    }

    auto secantMethod() -> void
    {
        auto x0 = leftBorder;
        auto x1 = rightBorder;
        std::size_t it = 0;

        auto f = [this](float x) {
            return root->compute(x, 0.0);
        };

        while (std::abs(f(x1)) > epsilon && it < maxIterations) {
            auto new_x = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0));

            ++it;

            sphereInstances.models.emplace_back(
                mv::Color::RED,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            new_x,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            x0 = x1;
            x1 = new_x;
        }

        fmt::println("{} {} {}", it, x1, f(x1));
    }

    auto iterationsMethod() -> void
    {
        auto f = [this](float x) {
            return root->compute(x, 0.0);
        };

        auto xi = (leftBorder + rightBorder) / 2;
        std::size_t it = 0;

        while (std::abs(f(xi)) > epsilon && it < maxIterations) {
            xi = xi - f(xi) / root->derivationX(xi, 0.0);
            ++it;

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
        }


        fmt::println("{} {} {}", it, xi, f(xi));
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

        prism.loadData();
        prism.vbo.bind();
        prism.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        functionGraph.loadData();
        functionGraph.vbo.bind();
        functionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        dftGraph.loadData();
        dftGraph.vbo.bind();
        dftGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstances.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);

        sphereInstances.loadData();
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::InputText("Equation", &input);

        if (ImGui::Button("Solve")) {
            root = mvl::constructRoot(input);
            constructPlot();
        }

        ImGui::SliderFloat("Left border", &leftBorder, -20.0F, rightBorder);
        ImGui::SliderFloat("Right border", &rightBorder, leftBorder, 20.0F);

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setVec4("elementColor", mv::Color::FOREST);
        prism.drawAt(colorShader, {leftBorder, 0.0F, 0.0F});

        colorShader.setVec4("elementColor", mv::Color::NAVY);
        prism.drawAt(colorShader, {rightBorder, 0.0F, 0.0F});

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);
        functionGraph.draw();

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
