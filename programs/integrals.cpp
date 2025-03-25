#include <ccl/runtime.hpp>
#include <cmath>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/linalg/linspace.hpp>
#include <mv/application_2d.hpp>
#include <mv/application_3d.hpp>
#include <mv/color.hpp>
#include <mv/gl/axes_2d.hpp>
#include <mv/gl/instances_holder.hpp>
#include <mv/gl/shaders/color_shader.hpp>
#include <mv/gl/shaders/shader_with_positioning.hpp>
#include <mv/gl/shape/plot_2d.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/shader.hpp>
#include <mvl/mvl.hpp>
#include <valarray>

class Integrals final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static std::size_t maxIterations = 102'400;

    constexpr static glm::vec3 defaultCameraPosition{0.0F, 0.0F, 10.0F};

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader &colorShader = mv::gl::getColorShader();

    mv::Shader &shaderWithPositioning = mv::gl::getShaderWithPositioning();

    mv::gl::shape::Axes2D plot{12, 0.009F};

    mv::gl::shape::Plot2D functionGraph;

    mv::gl::shape::Prism prism{0.03F, 20.0F, 4};

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1000);
    std::valarray<float> functionY = functionX;

    float sphereRadius = 0.1F;

    std::string input = "x^3-3*x^2+7*x-10";
    std::string tmpInput = input;

    float leftBorder = -5.0F;
    float rightBorder = 5.0F;

    float epsilon = 1e-5F;

    double rectangleMethodLeft = 0.0;
    double rectangleMethodMiddle = 0.0;
    double rectangleMethodRight = 0.0;

    double trapezoidalResult = 0.0;
    double simpsonsResult = 0.0;

    std::size_t rectangleMethodLeftPartitionsCount = 0;
    std::size_t rectangleMethodMiddlePartitionsCount = 0;
    std::size_t rectangleMethodRightPartitionsCount = 0;
    std::size_t trapezoidalMethodPartitionsCount = 0;
    std::size_t simpsonsMethodPartitionsCount = 0;

    ccl::parser::ast::SharedNode<mvl::ast::MathNode> root;

    float lineThickness = 0.03F;

public:
    using Application2D::Application2D;

    [[nodiscard]] auto f(const double x) const -> double
    {
        if (root == nullptr) {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return root->compute(x, 0.0);
    }

    auto compute() -> isl::Task<>
    {
        root = mvl::constructRoot(input);

        if (root != nullptr) {
            constructPlot().await();
        }

        co_return;
    }

    auto constructPlot() -> isl::Task<>
    {
        if (root == nullptr) {
            co_return;
        }

        auto new_y = functionX;
        auto left_point_method = computeUsingRectangleMethodLeft();
        auto middle_point_method = computeUsingRectangleMethodMiddle();
        auto right_point_method = computeUsingRectangleMethodRight();
        auto trapezoidal_method = computeUsingTrapezoidalMethod();
        auto simpson_method = computeUsingSimpsonMethod();

        for (std::size_t i = 0; i != new_y.size(); ++i) {
            new_y[i] = static_cast<float>(f(static_cast<double>(new_y[i])));
        }

        submit([this, y = std::move(new_y)]() mutable {
            functionY = std::move(y);
            functionGraph.fill(functionX, functionY, lineThickness);
            functionGraph.loadData();
        });

        co_await left_point_method;
        co_await middle_point_method;
        co_await right_point_method;
        co_await trapezoidal_method;
        co_await simpson_method;

        co_return;
    }

    template<std::invocable<double, double> F>
    [[nodiscard]] auto
        rectangleMethod(const std::valarray<double> &partitions, F &&select_from_segment) const
        -> double
    {
        auto result = 0.0;
        auto previous_x = partitions[0];
        const auto delta = partitions[1] - partitions[0];

        for (auto current_x : partitions | std::views::drop(1)) {
            auto f_value = f(select_from_segment(previous_x, current_x));

            if (std::isinf(f_value)) {
                previous_x = current_x;
                continue;
            }

            result += f_value * delta;
            previous_x = current_x;
        }

        return result;
    }

    [[nodiscard]] auto trapezoidalMethod(const std::valarray<double> &partition) const -> double
    {
        auto result = 0.0;
        auto previous_x = partition[0];
        const auto delta = partition[1] - partition[0];
        auto previous_f_value = f(previous_x);

        for (const auto current_x : partition | std::views::drop(1)) {
            const auto current_f_value = f(current_x);

            if (std::isinf(previous_f_value) || std::isinf(current_f_value)) {
                previous_f_value = current_f_value;
                previous_x = current_x;
                continue;
            }

            result += (previous_f_value + current_f_value) * delta / 2.0;

            previous_x = current_x;
            previous_f_value = current_f_value;
        }

        return result;
    }

    [[nodiscard]] auto simpsonsMethod(const std::valarray<double> &partition) const -> double
    {
        auto result = 0.0;
        auto previous_x = partition[0];
        auto delta = partition[1] - partition[0];
        auto previous_f = f(previous_x);

        for (const auto current_x : partition | std::views::drop(1)) {
            const auto current_f = f(current_x);
            const auto middle_f = f(std::midpoint(previous_x, current_x));

            if (std::isinf(previous_f) || std::isinf(current_f) || std::isinf(middle_f)) {
                previous_f = current_f;
                previous_x = current_x;
                continue;
            }

            result += (previous_f + 4.0 * middle_f + current_f) * delta / 6.0;

            previous_x = current_x;
            previous_f = current_f;
        }

        return result;
    }

    auto computeUsingRectangleMethodLeft() -> isl::Task<>
    {
        std::size_t n = 4;
        auto previous_value = 0.0;
        auto current_value = std::numeric_limits<double>::infinity();

        while (std::abs(previous_value - current_value) > epsilon && n < maxIterations) {
            previous_value = current_value;

            const auto partitions = isl::linalg::linspace<double>(
                static_cast<double>(leftBorder), static_cast<double>(rightBorder), n);

            current_value = rectangleMethod(partitions, [](const double a, double) {
                return a;
            });

            n *= 2;
        }

        submit([current_value, n, this]() {
            rectangleMethodLeftPartitionsCount = n;
            rectangleMethodLeft = current_value;
        });

        co_return;
    }

    auto computeUsingRectangleMethodMiddle() -> isl::Task<>
    {
        std::size_t n = 4;
        auto previous_value = 0.0;
        auto current_value = std::numeric_limits<double>::infinity();

        while (std::abs(previous_value - current_value) > epsilon && n < maxIterations) {
            previous_value = current_value;

            auto partitions = isl::linalg::linspace<double>(
                static_cast<double>(leftBorder), static_cast<double>(rightBorder), n);

            current_value = rectangleMethod(partitions, [](const double a, const double b) {
                return std::midpoint(a, b);
            });

            n *= 2;
        }

        submit([current_value, n, this]() {
            rectangleMethodMiddlePartitionsCount = n;
            rectangleMethodMiddle = current_value;
        });

        co_return;
    }

    auto computeUsingRectangleMethodRight() -> isl::Task<>
    {
        std::size_t n = 4;
        auto previous_value = 0.0;
        auto current_value = std::numeric_limits<double>::infinity();

        while (std::abs(previous_value - current_value) > epsilon && n < maxIterations) {
            previous_value = current_value;

            const auto partitions = isl::linalg::linspace<double>(
                static_cast<double>(leftBorder), static_cast<double>(rightBorder), n);

            current_value = rectangleMethod(partitions, [](double, const double b) {
                return b;
            });

            n *= 2;
        }

        submit([current_value, n, this]() {
            rectangleMethodRightPartitionsCount = n;
            rectangleMethodRight = current_value;
        });

        co_return;
    }

    auto computeUsingTrapezoidalMethod() -> isl::Task<>
    {
        std::size_t n = 4;
        auto previous_value = std::numeric_limits<double>::infinity();
        auto current_value = 0.0;

        while (std::abs(previous_value - current_value) > epsilon && n < maxIterations) {
            previous_value = current_value;

            auto partitions = isl::linalg::linspace<double>(
                static_cast<double>(leftBorder), static_cast<double>(rightBorder), n);

            current_value = trapezoidalMethod(partitions);
            
            n *= 2;
        }

        submit([current_value, n, this]() {
            trapezoidalMethodPartitionsCount = n;
            trapezoidalResult = current_value;
        });

        co_return;
    }

    auto computeUsingSimpsonMethod() -> isl::Task<>
    {
        std::size_t n = 10;
        auto previous_value = 0.0;
        auto current_value = std::numeric_limits<double>::infinity();

        while (std::abs(previous_value - current_value) > epsilon && n < maxIterations) {
            previous_value = current_value;

            auto partitions = isl::linalg::linspace<double>(
                static_cast<double>(leftBorder), static_cast<double>(rightBorder), n);

            current_value = simpsonsMethod(partitions);
            n *= 2;
        }

        submit([current_value, n, this]() {
            simpsonsMethodPartitionsCount = n;
            simpsonsResult = current_value;
        });

        co_return;
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        prism.loadData();
        prism.vbo.bind();
        prism.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        functionGraph.loadData();
        functionGraph.vbo.bind();
        functionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);
    }

    auto update() -> void override
    {
        Application2D::update();

        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::Text(
            "Left point result: %f, partitions count: %zu\n"
            "Middle point result %f, partitions count: %zu\n"
            "Right point result %f, partitions count: %zu\n"
            "Trapezoidal result %f, partitions count: %zu\n"
            "Simpson result %f, partitions count: %zu",
            rectangleMethodLeft, rectangleMethodLeftPartitionsCount, rectangleMethodMiddle,
            rectangleMethodMiddlePartitionsCount, rectangleMethodRight,
            rectangleMethodRightPartitionsCount, trapezoidalResult,
            trapezoidalMethodPartitionsCount, simpsonsResult, simpsonsMethodPartitionsCount);

        ImGui::InputText("Equation", &tmpInput);

        if (ImGui::Button("Solve")) {
            input = tmpInput;
            ccl::runtime::getGlobalThreadPool().launch(compute());
        }

        ImGui::SliderFloat("Epsilon", &epsilon, 1e-6F, 1e-2F, "%.3e");

        ImGui::SliderFloat("Left border", &leftBorder, -20.0F, rightBorder, "%.3e");
        ImGui::SliderFloat("Right border", &rightBorder, leftBorder, 20.0F, "%.3e");

        if (ImGui::SliderFloat("Line width", &lineThickness, 0.001F, 0.1F)) {
            functionGraph.fill(functionX, functionY, lineThickness);
            functionGraph.loadData();
        }

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
    Integrals application{1000, 800, "Equation solver", 2};
    application.run();
    return 0;
}
