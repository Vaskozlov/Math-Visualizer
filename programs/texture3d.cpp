#include "mv/gl/texture3d.hpp"

#include "mv/gl/shape/rectangle.hpp"

#include <ccl/runtime.hpp>
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
#include <mv/gl/shaders/color_shader.hpp>
#include <mv/gl/shaders/shader_with_positioning.hpp>
#include <mv/gl/shape/plot_2d.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/shader.hpp>
#include <mvl/mvl.hpp>
#include <valarray>
#include <vector>

class Texture3D final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static glm::vec3 defaultCameraPosition{0.0F, 0.0F, 10.0F};
    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};
    std::vector<float> data;
    mv::Shader *colorShader = mv::gl::getColorShader();
    mv::Shader *linearFixedLevelShader = mv::gl::getTexture3DLinearShader();
    mv::gl::shape::Rectangle rectangle{0.0F, 0.0F, 1.0F, 1.0F};
    mv::gl::Texture3D texture{
        nullptr,
        0,
        0,
        0,
        mv::gl::TextureMode::F32,
        mv::gl::TextureWrapMode::CLAMP_TO_EDGE,
        mv::gl::TextureScaleFormat::NEAREST,
    };

    ImFont *font{};
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::array<bool, 3> frozenAxes{false, false, true};
    glm::vec3 frozenAxesPosition{0.0F, 0.0F, 0.0F};

    mv::gl::shape::Axes2D plot{12, 0.009F};

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);

        // for (std::size_t i = 0; i < data.size(); ++i) {
        //     data[i] = static_cast<float>(i) / static_cast<float>(data.size());
        // }

        constexpr std::size_t data_size = 10;
        data.resize(data_size * data_size * data_size);

        for (std::size_t z = 0; z < data_size; z++) {
            for (std::size_t y = 0; y < data_size; y++) {
                for (std::size_t x = 0; x < data_size; x++) {
                    auto &value = data[z * data_size * data_size + y * data_size + x];

                    value = static_cast<float>(z) / static_cast<float>(data_size);
                }
            }
        }

        rectangle.loadData();
        rectangle.vbo.bind();
        rectangle.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        rectangle.vao.bind(1, 2, GL_FLOAT, 2 * sizeof(float), 0);

        texture.resize(static_cast<const void *>(data.data()), data_size, data_size, data_size);
        texture.updateTexture();

        // plot.loadData();
        // plot.vbo.bind();
        // plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        updateShaderFixedLevelMask();
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

        if (ImGui::Button("Center camera")) {
            camera.setPosition(defaultCameraPosition);
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        if (ImGui::Checkbox("x-level", &frozenAxes[0])) {
            if (frozenAxes[0]) {
                frozenAxes[1] = frozenAxes[2] = false;
            }

            updateShaderFixedLevelMask();
        }

        if (ImGui::Checkbox("y-level", &frozenAxes[1])) {
            if (frozenAxes[1]) {
                frozenAxes[0] = frozenAxes[2] = false;
            }

            updateShaderFixedLevelMask();
        }

        if (ImGui::Checkbox("z-level", &frozenAxes[2])) {
            if (frozenAxes[2]) {
                frozenAxes[0] = frozenAxes[1] = false;
            }

            updateShaderFixedLevelMask();
        }

        showFixedAxisSlider();

        ImGui::PopFont();
        ImGui::End();

        // colorShader->use();
        // colorShader->setMat4("projection", getResultedViewMatrix());
        // colorShader->setMat4(
        //     "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        // colorShader->setVec4("elementColor", mv::Color::BLACK);

        // plot.draw();

        linearFixedLevelShader->use();

        if (frozenAxes[0]) {
            linearFixedLevelShader->setFloat("fixedLevelValue", frozenAxesPosition.x);
        } else if (frozenAxes[1]) {
            linearFixedLevelShader->setFloat("fixedLevelValue", frozenAxesPosition.y);
        } else {
            linearFixedLevelShader->setFloat("fixedLevelValue", frozenAxesPosition.z);
        }

        linearFixedLevelShader->setMat4("projection", getResultedViewMatrix());
        linearFixedLevelShader->setMat4("model", glm::scale(glm::mat4(1.0F), glm::vec3{5.0F}));

        texture.bind();
        rectangle.draw();
    }

    auto showFixedAxisSlider() -> void
    {
        if (frozenAxes[0]) {
            ImGui::SliderFloat("x-value", &frozenAxesPosition.x, 0.0F, 1.0F, "%.3f");
        } else if (frozenAxes[1]) {
            ImGui::SliderFloat("y-value", &frozenAxesPosition.y, 0.0F, 1.0F, "%.3f");
        } else {
            ImGui::SliderFloat("z-value", &frozenAxesPosition.z, 0.0F, 1.0F, "%.3f");
        }
    }

    auto updateShaderFixedLevelMask() const -> void
    {
        linearFixedLevelShader->use();
        auto vec = glm::vec3(0);

        if (frozenAxes[0]) {
            vec.x = std::numeric_limits<float>::quiet_NaN();
        } else if (frozenAxes[1]) {
            vec.y = std::numeric_limits<float>::quiet_NaN();
        } else {
            vec.z = std::numeric_limits<float>::quiet_NaN();
        }

        linearFixedLevelShader->setVec3("fixedLevel", vec);
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

auto main() -> int
{
    Texture3D application{1000, 800, "Texture 3d", 2};
    application.run();

    return 0;
}
