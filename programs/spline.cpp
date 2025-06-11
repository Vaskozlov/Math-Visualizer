#include <ccl/runtime.hpp>
#include <cstdlib>
#include <future>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/linalg/lagrange.hpp>
#include <isl/linalg/linspace.hpp>
#include <isl/linalg/spline.hpp>
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
#include <valarray>
#include <vector>

class SplineInterpolation final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static double plotEpsilon = 5e-2;
    constexpr static std::size_t maxIterations = 1000;

    constexpr static glm::vec3 defaultCameraPosition{0.0F, 0.0F, 10.0F};

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader colorShader = getColorShader();
    mv::Shader shaderWithPositioning = getShaderWithPositioning();

    mv::gl::shape::Axes2D plot{12, 0.009F};

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::shape::Sphere plotSphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> iterationsSpheres;
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> plotSpheres;

    ImFont *font{};
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1500);
    std::valarray<float> functionY = functionX;
    std::valarray<float> functionY2 = functionY;

    mv::gl::shape::Plot2D functionGraph;
    mv::gl::shape::Plot2D functionGraph2;

    float sphereRadius = 0.1F;
    int dataCount = 6;

    std::vector<float> dataX{0.0F, 1.0F, 2.0F, 3.0F, 4.0F, 5.0F};
    std::vector<float> dataY{0.0F, 1.5F, 0.3F, 2.8F, 1.1F, 3.0F};

    auto addSphere(
        std::vector<mv::gl::InstanceParameters> &container, const mv::Color color,
        const glm::vec3 position, const float scale = 1.0F) const -> void
    {
        container.emplace_back(
            color,
            glm::scale(glm::translate(glm::mat4(1.0F), position), glm::vec3{sphereRadius * scale}));
    }

    float inputValue{};
    float resultValueL{};
    float resultVaue{};

public:
    using Application2D::Application2D;

    static auto countSplineValue(
        const std::span<const float> x,
        const std::vector<isl::interpolation::CubicSplineCoefficients<float>> &spline_coefficients,
        const float value)
    {
        const auto it = std::ranges::lower_bound(x, value);
        const auto index = static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(
            std::distance(x.begin(), it) - 1, 0, spline_coefficients.size() - 1));

        const auto &coefficient = spline_coefficients[index];
        const auto h = value - x[index];

        return coefficient.a + coefficient.b * h + coefficient.c * h * h
               + coefficient.d * h * h * h;
    }

    auto computeFunction() -> void
    {
        const auto spline_coefficients = isl::interpolation::createCubicSpline(
            std::span<const float>(dataX.data(), dataX.size()),
            std::span<const float>(dataY.data(), dataY.size()));

        const auto left = dataX.front() - 1.0F;
        const auto right = dataX.back() + 1.0F;

        const auto step = (right - left) / 100.0F;
        auto x = left;

        functionX.resize(100);
        functionY.resize(100);
        functionY2.resize(100);

        iterationsSpheres.models.clear();

        for (std::size_t i = 0; i < dataX.size(); ++i) {
            addSphere(iterationsSpheres.models, mv::Color::GREEN, {dataX[i], dataY[i], 0.01F});
        }

        for (isl::ssize_t i = 0; i < 100; ++i, x += step) {
            const auto it = std::lower_bound(dataX.begin(), dataX.end(), x);
            const auto index = static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(
                std::distance(dataX.begin(), it) - 1, 0, spline_coefficients.size() - 1));

            const auto &coefficient = spline_coefficients[index];
            const auto h = x - dataX[index];

            functionX[i] = x;
            functionY[i] = coefficient.a + coefficient.b * h + coefficient.c * h * h
                           + coefficient.d * h * h * h;

            functionY2[i] = isl::interpolation::lagrange<float>(dataX, dataY, x);
        }

        functionGraph.fill(functionX, functionY);
        functionGraph2.fill(functionX, functionY2);
        functionGraph.loadData();
        functionGraph2.loadData();
        iterationsSpheres.loadData();
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        iterationsSpheres.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        plotSphere.vbo.bind();
        plotSphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        plotSpheres.vbo.bind();
        plotSphere.vao.bindInstanceParameters(1, 1);

        functionGraph.loadData();
        functionGraph.vbo.bind();
        functionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        functionGraph2.loadData();
        functionGraph2.vbo.bind();
        functionGraph2.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);

        dataX.resize(dataCount);
        dataY.resize(dataCount);
    }

    auto update() -> void override
    {
        Application2D::update();

        fmt::format_to_n(
            imguiWindowBuffer.data(),
            imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle",
            ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        if (ImGui::BeginTable("Input table", dataCount + 1)) {

            for (std::size_t row = 0; row < 2; ++row) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text(row == 0 ? "x" : "y");

                for (std::size_t col = 0; col < dataCount; ++col) {
                    ImGui::TableNextColumn();
                    auto label = fmt::format("###cell_input_{}_{}", row, col);
                    ImGui::InputFloat(label.c_str(), row == 0 ? &dataX[col] : &dataY[col]);
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::InputInt("Data count", &dataCount, 1)) {
            if (dataCount < 3) {
                dataCount = 3;
            }

            dataX.resize(dataCount);
            dataY.resize(dataCount);
        }

        if (ImGui::Button("Draw")) {
            computeFunction();
        }

        ImGui::InputFloat("Value ", &inputValue);

        if (ImGui::Button("Count")) {
            auto spline_coeff = isl::interpolation::createCubicSpline<float>(dataX, dataY);
            resultVaue = countSplineValue(dataX, spline_coeff, inputValue);
            resultValueL = isl::interpolation::lagrange<float>(dataX, dataY, inputValue);
        }

        ImGui::Text("Result %f %f", resultVaue, resultValueL);

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setVec4("elementColor", mv::Color::PINK);

        colorShader.setMat4(
            "model", glm::scale(glm::translate(glm::mat4(1.0f), {}), glm::vec3(sphereRadius)));

        sphere.draw();

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);
        functionGraph.draw();

        colorShader.setVec4("elementColor", mv::Color::NAVY);
        functionGraph2.draw();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), iterationsSpheres.models.size());

        plotSphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), plotSpheres.models.size());

        if (ImGui::Button("Center camera")) {
            camera.setPosition(defaultCameraPosition);
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        ImGui::PopFont();
        ImGui::End();
    }

    auto onMouseClick(int button, int action, int mods) -> void override
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int width, height;
            glfwGetWindowSize(window, &width, &height);
        }
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

static auto countSplineValue(
    const std::span<const float> x,
    const std::vector<isl::interpolation::CubicSplineCoefficients<float>> &spline_coefficients,
    const float value)
{
    const auto it = std::ranges::lower_bound(x, value);
    const auto index = static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(
        std::distance(x.begin(), it) - 1, 0, spline_coefficients.size() - 1));

    const auto &coefficient = spline_coefficients[index];
    const auto h = value - x[index];

    return coefficient.a + coefficient.b * h + coefficient.c * h * h + coefficient.d * h * h * h;
}

std::vector<float> dataX{0.0F, 1.0F, 2.0F, 3.0F, 4.0F, 5.0F};
std::vector<float> dataY{0.0F, 1.5F, 0.3F, 2.8F, 1.1F, 3.0F};

auto main(int, const char *argv[]) -> int
{
    SplineInterpolation application{
        std::filesystem::path(std::getenv("APPDIR")) / "usr" / "local",
        1000,
        800,
        "System of equations",
        2};
    application.run();

    // auto spline_coefficients = isl::interpolation::createCubicSpline<float>(dataX, dataY);
    // fmt::println("{}", countSplineValue(dataX, spline_coefficients, 4.5F));

    return 0;
}
