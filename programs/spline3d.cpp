#include "mv/gl/axes_3d.hpp"
#include "mv/gl/shape/function_3d.hpp"

#include <algorithm>
#include <ccl/runtime.hpp>
#include <future>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/linalg/lagrange.hpp>
#include <isl/linalg/linspace.hpp>
#include <isl/linalg/spline.hpp>
#include <iterator>
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

class SplineInterpolation3D final : public mv::Application3D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static double plotEpsilon = 5e-2;
    constexpr static std::size_t maxIterations = 1500;

    constexpr static glm::vec3 defaultCameraPosition{10.0F, 10.0F, 10.0F};

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader colorShader = getColorShader();
    mv::Shader shaderWithPositioning = getShaderWithPositioning();

    mv::gl::shape::Axes3D plot{12};

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::shape::Sphere plotSphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> iterationsSpheres;
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> plotSpheres;

    ImFont *font{};
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1500);
    std::valarray<float> functionY = functionX;
    std::valarray<float> functionXInterp = functionX;
    std::valarray<float> functionXInterp2 = functionX;
    std::valarray<float> functionYInterp = functionX;
    std::valarray<float> functionYInterp2 = functionX;

    mv::gl::shape::Function3D functionPlot;

    float sphereRadius = 0.1F;
    int dataCountX = 6;
    int dataCountY = 6;

    std::vector<float> dataX{
        -8.0F,
        -4.0F,
        0.0F,
        4.0F,
        8.0F,
        10.0F,
    };

    std::vector<float> dataY{
        -8.0F,
        -4.0F,
        0.0F,
        4.0F,
        8.0F,
        10.0F,
    };

    /*
    first index is y
    second index is x
    */
    std::vector<std::vector<float>> dataZ{
        {1,     2,    3,     5,     7, 10   },
        {2,     3,    7,     2,     5, 6    },
        {3,     4,    1,     -1,    3, 1.5F },
        {1.5F,  2.0F, 0.75F, -1.2F, 0, -1.0F},
        {-1.0F, 0.0F, 2.75F, -2.2F, 0, -5.0F},
        {-1.0F, 0.0F, 2.75F, -2.2F, 0, -5.0F},
    };

public:
    using Application3D::Application3D;

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

    static auto partInterpolation(
        const std::span<const float> x,
        const std::span<const float>
            y,
        const std::size_t result_length,
        const float left,
        const float right) -> std::vector<float>
    {
        const auto step = (right - left) / static_cast<float>(result_length);
        auto value = left;

        std::vector<float> result(result_length);

        const auto spline_coefficients = isl::interpolation::createCubicSpline(x, y);

        for (std::size_t i = 0; i < result_length; ++i, value += step) {
            result[i] = countSplineValue(x, spline_coefficients, value);
        }

        return result;
    }

    auto addSphere(
        std::vector<mv::gl::InstanceParameters> &container, const mv::Color color,
        const glm::vec3 position, const float scale = 1.0F) const -> void
    {
        container.emplace_back(
            color,
            glm::scale(glm::translate(glm::mat4(1.0F), position), glm::vec3{sphereRadius * scale}));
    }

    auto computeFunction() -> void
    {
        plotSpheres.models.clear();

        std::vector<std::vector<float>> interpolation_along_x;
        std::vector<std::vector<float>> grid;
        interpolation_along_x.reserve(dataY.size());

        const auto x_begin = dataX.front() - 1.0F;
        const auto x_end = dataX.back() + 1.0F;

        const auto [y_begin_it, y_end_it] = std::ranges::minmax_element(dataY);

        const auto y_begin = *y_begin_it - 1.0F;
        const auto y_end = *y_end_it + 1.0F;

        constexpr std::size_t length = 1200;

        for (std::size_t i = 0; i < dataY.size(); ++i) {
            for (std::size_t j = 0; j < dataX.size(); ++j) {
                addSphere(plotSpheres.models, mv::Color::NAVY, {dataX[j], dataZ[i][j], dataY[i]});
            }
        }

        plotSpheres.loadData();

        // first we interpolate with const y
        for (std::size_t i = 0; i < dataZ.size(); ++i) {
            auto interpolation_result_constant_y =
                partInterpolation(dataX, dataZ[i], length, x_begin, x_end);

            interpolation_along_x.emplace_back(std::move(interpolation_result_constant_y));
        }

        for (std::size_t i = 0; i < length; ++i) {
            std::vector<float> interpolation_data;

            for (auto &j : interpolation_along_x) {
                interpolation_data.emplace_back(j[i]);
            }

            auto interpolation_result_constant_x =
                partInterpolation(dataY, interpolation_data, length, y_begin, y_end);

            grid.emplace_back(std::move(interpolation_result_constant_x));
        }

        auto real_x = partInterpolation(dataX, dataX, length, x_begin, x_end);

        functionPlot.evaluatePoints([&](glm::vec2 point) {
            auto x_raw_index =
                std::round((point.x - x_begin) / (x_end - x_begin) * static_cast<float>(length));
            auto y_raw_index =
                std::round((point.y - y_begin) / (y_end - y_begin) * static_cast<float>(length));

            x_raw_index =
                static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(x_raw_index, 0, length - 1));

            y_raw_index =
                static_cast<std::size_t>(std::clamp<std::ptrdiff_t>(y_raw_index, 0, length - 1));

            return grid[x_raw_index][y_raw_index];
        }, x_begin, y_begin, -20.0F, x_end, y_end, 20.0F);

        functionPlot.loadData();
    }

    auto init() -> void override
    {
        Application3D::init();
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

        functionPlot.vbo.bind();
        functionPlot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        // functionGraph.loadData();
        // functionGraph.vbo.bind();
        // functionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
        //
        // functionGraph2.loadData();
        // functionGraph2.vbo.bind();
        // functionGraph2.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);
        camera.setPosition(defaultCameraPosition);

        dataX.resize(dataCountX);
        dataY.resize(dataCountY);
    }

    auto update() -> void override
    {
        Application3D::update();

        fmt::format_to_n(
            imguiWindowBuffer.data(),
            imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle",
            ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        if (ImGui::BeginTable("Input table", std::max(dataCountX, dataCountY) + 1)) {

            for (std::size_t row = 0; row < 2; ++row) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text(row == 0 ? "x" : "y");

                auto l = row == 0 ? dataCountX : dataCountY;

                for (std::size_t col = 0; col < l; ++col) {
                    ImGui::TableNextColumn();
                    auto label = fmt::format("###cell_input_{}_{}", row, col);
                    ImGui::InputFloat(
                        label.c_str(), row == 0 ? dataX.data() + col : dataY.data() + col);
                }
            }

            for (std::size_t i = 0; i < dataCountY; ++i) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("y = %.2f", dataY[i]);

                for (std::size_t j = 0; j < dataCountX; ++j) {
                    ImGui::TableNextColumn();
                    auto label = fmt::format("###cell_input_2_{}_{}", i, j);
                    ImGui::InputFloat(label.c_str(), &dataZ[i][j]);
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::InputInt("Data count x", &dataCountX, 1)) {
            if (dataCountX < 3) {
                dataCountX = 3;
            }

            dataX.resize(dataCountX);
            dataY.resize(dataCountY);

            for (std::size_t i = 0; i < dataCountY; ++i) {
                dataZ[i].resize(dataCountX);
            }
        }

        if (ImGui::InputInt("Data count y", &dataCountY, 1)) {
            if (dataCountY < 3) {
                dataCountY = 3;
            }

            dataX.resize(dataCountX);
            dataY.resize(dataCountY);

            dataZ.resize(dataCountY, std::vector<float>(dataCountX));
        }

        if (ImGui::Button("Draw")) {
            computeFunction();
        }

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
        // functionGraph.draw();
        functionPlot.draw();

        colorShader.setVec4("elementColor", mv::Color::NAVY);
        // functionGraph2.draw();

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

        Application3D::processInput();

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
        Application3D::onMouseRelativeMovement(delta_x, delta_y);
        // const auto scale = camera.getZoom() / 9000.0F;

        // camera.relativeMove(camera.getUp() * static_cast<float>(delta_y) * scale);
        // camera.relativeMove(camera.getRight() * static_cast<float>(delta_x) * scale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = static_cast<double>(camera.getZoom()) / 180.0;
        Application3D::onScroll(x_offset * scale, y_offset * scale);
    }
};

auto main(int, const char *argv[]) -> int
{
    SplineInterpolation3D application{argv[0], 1000, 800, "System of equations", 2};
    application.run();
    return 0;
}
