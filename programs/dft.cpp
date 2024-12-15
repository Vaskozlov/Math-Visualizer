#include <battery/embed.hpp>
#include <complex>
#include <imgui.h>
#include <isl/linalg/linspace.hpp>
#include <mv/application_2d.hpp>
#include <mv/application_3d.hpp>
#include <mv/gl/axes_2d.hpp>
#include <mv/gl/instances_holder.hpp>
#include <mv/gl/shape/plot_2d.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/shader.hpp>
#include <numbers>
#include <valarray>

class DftApplication final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static std::array<std::string_view, 8> frequenciesNames = {
        "1 Гц", "2 Гц", "3 Гц", "4 Гц", "5 Гц", "6 Гц", "7 Гц", "8 Гц",
    };

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader colorShader = mv::Shader{
        {b::embed<"resources/shaders/colored_shader.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader shaderWithPositioning = mv::Shader{
        {b::embed<"resources/shaders/static_instance.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::gl::shape::Axes2D plot{3};

    mv::gl::shape::Plot2D resultedSignalGraph;
    mv::gl::shape::Plot2D dftGraph;

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> sphereInstances;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    float frequency = 2.0F;
    float lineWidth = 0.02F;
    float sphereRadius = 0.023F;

    std::array<bool, 8> selectedFrequencies{false, true, false, false, false, false, false, false};

    int pointsCount = 2048 * 4;
    int pointsDrawDivider = 32;

    std::valarray<float> xLinearSpace =
        isl::linalg::linspace<float>(0, 10, static_cast<size_t>(pointsCount));

    std::valarray<float> signalLinearSpace = std::valarray<float>(xLinearSpace.size());

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        resultedSignalGraph.loadData();
        resultedSignalGraph.vbo.bind();
        resultedSignalGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        dftGraph.loadData();
        dftGraph.vbo.bind();
        dftGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstances.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        ImGui::StyleColorsLight();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        setClearColor({0.8F, 0.8F, 0.8F, 1.0F});

        calculateSignal();
        calculateDft();
    }

    auto calculateSignal() -> void
    {
        signalLinearSpace = std::valarray<float>(xLinearSpace.size());

        for (size_t i = 0; i != selectedFrequencies.size(); ++i) {
            if (selectedFrequencies.at(i)) {
                const auto freq = static_cast<float>(i + 1);

                for (std::size_t j = 0; j != xLinearSpace.size(); ++j) {
                    signalLinearSpace[j] +=
                        std::sin(2.0F * std::numbers::pi_v<float> * freq * xLinearSpace[j]) + 1;
                }
            }
        }

        resultedSignalGraph.fill(xLinearSpace, signalLinearSpace, 0.015F);
        resultedSignalGraph.loadData();
    }

    auto calculateDft() -> void
    {
        sphereInstances.models.clear();

        auto x2 = std::valarray<float>(xLinearSpace.size());
        auto y2 = std::valarray<float>(xLinearSpace.size());
        std::complex<float> center_of_mass{};

        for (std::size_t i = 0; i != xLinearSpace.size(); ++i) {
            auto point =
                signalLinearSpace[i] * std::exp(
                                           std::complex(0.0F, 2.0F) * std::numbers::pi_v<float> *
                                           frequency * xLinearSpace[i]);

            center_of_mass += point;
            x2[i] = point.imag();
            y2[i] = point.real();
        }

        center_of_mass /= static_cast<float>(xLinearSpace.size());

        for (std::size_t i = 0; i < x2.size(); i += pointsDrawDivider) {
            sphereInstances.models.emplace_back(
                glm::vec4{0.1F, 0.8F, 0.1F, 1.0F},
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            x2[i],
                            y2[i],
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));
        }

        dftGraph.fill(x2, y2, lineWidth);

        sphereInstances.models.emplace_back(
            glm::vec4{0.0F, 0.0F, 1.0F, 1.0F},
            glm::scale(
                glm::translate(
                    glm::mat4(1.0F),
                    {
                        center_of_mass.imag(),
                        center_of_mass.real(),
                        0.01F,
                    }),
                glm::vec3{2.5F * sphereRadius}));

        sphereInstances.loadData();
        dftGraph.loadData();
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.01F}));
        colorShader.setVec4("elementColor", glm::vec4(0.0F, 0.0F, 0.0F, 1.0F));

        plot.draw();

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", glm::vec4(1.0F, 0.301F, 0.0F, 1.0F));
        dftGraph.draw();
        // resultedSignalGraph.draw();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), sphereInstances.models.size());

        if (ImGui::Button("Center camera")) {
            camera.setPosition(glm::vec3(0.0F, 0.0F, 2.0F));
        }

        if (ImGui::SliderFloat(
                "Frequency", &frequency, 1.0F,
                static_cast<float>(selectedFrequencies.size()) + 1.0F)) {
            calculateDft();
        }

        for (size_t i = 0; i != selectedFrequencies.size(); ++i) {
            if (ImGui::Checkbox(frequenciesNames[i].data(), &selectedFrequencies[i])) {
                calculateSignal();
                calculateDft();
            }

            if (i + 1 != selectedFrequencies.size() / 2 && i != selectedFrequencies.size() - 1) {
                ImGui::SameLine();
            }
        }

        if (ImGui::SliderFloat("Line width", &lineWidth, 0.01F, 0.1F)) {
            calculateDft();
        }

        if (ImGui::SliderFloat("Sphere radius", &sphereRadius, 0.01F, 0.1F)) {
            calculateDft();
        }

        if (ImGui::SliderInt("Points count", &pointsCount, 128, 2048 * 16)) {
            xLinearSpace =
                isl::linalg::linspace<float>(0, 10, static_cast<std::size_t>(pointsCount));
            calculateSignal();
            calculateDft();
        }

        if (ImGui::SliderInt("Sphere draw divider", &pointsDrawDivider, 1, 128)) {
            calculateDft();
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
    DftApplication application{1000, 800, "Dft", 2};
    application.run();
    return 0;
}
