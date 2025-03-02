#ifndef WATERFALL_HPP
#define WATERFALL_HPP

#include <battery/embed.hpp>
#include <isl/shared_lib_loader.hpp>
#include <mv/application_2d.hpp>
#include <mv/gl/shape/rectangle.hpp>
#include <mv/gl/texture.hpp>
#include <mv/gl/waterfall.hpp>
#include <mv/shader.hpp>

#include <atomic>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <random>

class Waterfall : public mv::Application2D
{
private:
    std::array<char, 128> imguiWindowBufferTitle{};
    std::vector<std::function<void()>> commandsPipe;

    mv::Shader waterfallShader = mv::Shader{
        {b::embed<"resources/shaders/waterfall/default_vertex.vert">().str()},
        {b::embed<"resources/shaders/waterfall/rgba_rgba.frag">().str()},
    };

    mv::Shader waterfallShaderHsvF32 = mv::Shader{
        {
            b::embed<"resources/shaders/waterfall/vertex_with_limits_f32.vert">().str(),
        },
        {
            b::embed<"resources/shaders/waterfall/hsv/f32_rgba.frag">().str(),
            b::embed<"resources/shaders/waterfall/hsv/hsv2rgb.glsl">().str(),
        },
    };

    mv::Shader waterfallShaderLinearU32 = mv::Shader{
        {
            b::embed<"resources/shaders/waterfall/vertex_with_limits_f32.vert">().str(),
        },
        {
            b::embed<"resources/shaders/waterfall/linear/u32_rgba.frag">().str(),
            b::embed<"resources/shaders/waterfall/linear/linear_rgb.glsl">().str(),
        },
    };

    static constexpr std::size_t imageSize = 512;

    mv::gl::Waterfall<std::uint32_t> powerWaterfall{imageSize, imageSize};
    mv::gl::Waterfall<mv::RGBA<std::uint8_t>> powerWaterfallMask{imageSize, imageSize};

    mv::gl::Waterfall<float> azimuthWaterfall{imageSize, imageSize};
    mv::gl::Waterfall<mv::RGBA<std::uint8_t>> azimuthWaterfallMask{imageSize, imageSize};

    std::string libraryPath;

    double pressTime = 0.0;
    ImFont *font;
    float fontScale = 0.33F;

    bool disableInput = false;

    mv::gl::shape::Rectangle powerMapSize{0.0F, 0.0F, 1.0F, 1.0F};
    mv::gl::shape::Rectangle azimuthMapSize{0.0F, 0.0F, 1.0F, 1.0F};

    isl::SharedLibLoader shaderLib;

    float imageWidthScale = 1.0F;
    float imageHeightScale = 1.0F;

    float azimuthMiddle = 180.0F;
    float azimuthSide = 180.0F;

    int powerLow = 0;
    int powerHigh = 100;

    std::mutex updateMutex;

    std::atomic_flag continueFlag{false};

public:
    static constexpr float minWidthScale = 1.0F;
    static constexpr float minHeightScale = 1.0F;

    static constexpr float maxWidthScale = 20.0F;
    static constexpr float maxHeightScale = 20.0F;

    static constexpr float azimuthMin = 0.0F;
    static constexpr float azimuthMax = 360.0F;

    static constexpr int minPower = 0;
    static constexpr int maxPower = 100;

    static constexpr mv::RGBA<uint8_t> white{255, 255, 255, 255};

    using Application2D::Application2D;

    auto updateAzimuthUniform() const -> void
    {
        waterfallShaderHsvF32.use();
        waterfallShaderHsvF32.setVec2(
            "minMaxValue", glm::vec2{azimuthMiddle - azimuthSide, azimuthMiddle + azimuthSide});
    }

    auto updatePowerUniform() const -> void
    {
        waterfallShaderLinearU32.use();
        waterfallShaderLinearU32.setVec2("minMaxValue", glm::vec2{powerLow, powerHigh});
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        ImGui::StyleColorsDark();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        powerMapSize.loadData();
        powerMapSize.vbo.bind();
        powerMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        powerMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        azimuthMapSize.loadData();
        azimuthMapSize.vbo.bind();
        azimuthMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        azimuthMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        camera.setPosition(glm::vec3(-1.0F, -1.0F, 1.0F));

        powerWaterfallMask.setRange(0, 255);
        azimuthWaterfall.reload();
        azimuthWaterfallMask.reload();

        powerWaterfall.reload();
        powerWaterfallMask.reload();

        waterfallShaderHsvF32.use();
        waterfallShaderHsvF32.setInt("texture1", 0);
        waterfallShaderHsvF32.setInt("texture2", 1);
        waterfallShaderHsvF32.setMat4("projection", glm::mat4(1.0F));

        waterfallShaderLinearU32.use();
        waterfallShaderLinearU32.setInt("texture1", 0);
        waterfallShaderLinearU32.setInt("texture2", 1);
        waterfallShaderLinearU32.setMat4("projection", glm::mat4(1.0F));

        updateAzimuthUniform();
        updatePowerUniform();
    }

    auto pollTask() -> void
    {
        const std::scoped_lock lock{updateMutex};

        while (!commandsPipe.empty()) {
            commandsPipe.back()();
            commandsPipe.pop_back();
        }
    }

    auto update() -> void override
    {
        pollTask();

        fmt::format_to_n(
            imguiWindowBufferTitle.data(), imguiWindowBufferTitle.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBufferTitle.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 3.0F, "%.3f");
        ImGui::InputText("Lib path: ", &libraryPath);

        if (ImGui::Button("Load")) {
            shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reload")) {
            if (!shaderLib.hasLibrary()) {
                shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();
            } else {
                shaderLib.reload();
            }

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        if (ImGui::SliderFloat("Azimuth middle", &azimuthMiddle, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderFloat("Azimuth side", &azimuthSide, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderInt("Power low", &powerLow, minPower, maxPower)) {
            updatePowerUniform();
        }

        if (ImGui::SliderInt("Power high", &powerHigh, minPower, maxPower)) {
            updatePowerUniform();
        }

        ImGui::SliderFloat("Image width scale", &imageWidthScale, minWidthScale, maxWidthScale);
        ImGui::SliderFloat("Image height scale", &imageHeightScale, minHeightScale, maxHeightScale);

        if (ImGui::Button("Continue")) {
            continueFlag.test_and_set();
            continueFlag.notify_one();
        }

        const auto camera_vec = camera.getPosition();

        azimuthWaterfall.bind(GL_TEXTURE0);
        azimuthWaterfallMask.bind(GL_TEXTURE1);

        waterfallShaderHsvF32.use();
        waterfallShaderHsvF32.setVec2("texOffset", glm::vec2(camera_vec.x, camera_vec.y) * 0.1F);

        azimuthMapSize.vao.bind();
        waterfallShaderLinearU32.setMat4(
            "model",
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3{-1.0F, -1.0F, 0.01F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        azimuthMapSize.draw();

        powerWaterfall.bind(GL_TEXTURE0);
        powerWaterfallMask.bind(GL_TEXTURE1);

        waterfallShaderLinearU32.use();
        waterfallShaderLinearU32.setVec2("texOffset", glm::vec2(camera_vec.x, camera_vec.y) * 0.1F);

        powerMapSize.vao.bind();
        waterfallShaderLinearU32.setMat4(
            "model",
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3{0.0F, -1.0F, 0.00F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        powerMapSize.draw();

        ImGui::PopFont();
        ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        if (!disableInput) {
            Application2D::processInput();
        }

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

    auto changeWidthScale(const float scale_difference)
    {
        imageWidthScale += scale_difference;
        imageWidthScale = std::clamp(imageWidthScale, minWidthScale, maxWidthScale);
    }

    auto changeHeightScale(const float scale_difference)
    {
        imageHeightScale += scale_difference;
        imageHeightScale = std::clamp(imageHeightScale, minHeightScale, maxHeightScale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const int v_key_state = glfwGetKey(window, GLFW_KEY_V);
        const int h_key_state = glfwGetKey(window, GLFW_KEY_H);
        const int shift_key_state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        const auto scale = static_cast<double>(camera.getZoom()) / 1000.0;

        if ((shift_key_state == GLFW_PRESS && h_key_state == GLFW_PRESS) ||
            (v_key_state == GLFW_FALSE && h_key_state == GLFW_FALSE)) {
            changeWidthScale((x_offset + y_offset) * scale);
        }

        if ((shift_key_state == GLFW_PRESS && v_key_state == GLFW_PRESS) ||
            (v_key_state == GLFW_FALSE && h_key_state == GLFW_FALSE)) {
            changeHeightScale((x_offset + y_offset) * scale);
        }
    }

    auto resizeImages(const std::size_t width, const std::size_t height) -> void
    {
        azimuthWaterfall.resize(width, height);
        powerWaterfall.resize(width, height);
        powerWaterfallMask.resize(width, height);
        azimuthWaterfallMask.resize(width, height);
    }

    auto drawRect(
        const std::size_t x, const std::size_t y, const std::size_t width,
        const std::size_t height) const -> void
    {
        azimuthWaterfallMask.drawRectangleBorder(x, y, width, height, white, 2);
        powerWaterfallMask.drawRectangleBorder(x, y, width, height, white, 2);
    }

    auto reloadImages() const -> void
    {
        azimuthWaterfall.reload();
        powerWaterfall.reload();
        azimuthWaterfallMask.reload();
        powerWaterfallMask.reload();
    }

    auto setPixel(
        const std::size_t x, const std::size_t y, const float azimuth,
        const std::uint32_t power) const -> void
    {
        azimuthWaterfall.setPixelValue(x, y, azimuth);
        powerWaterfall.setPixelValue(x, y, power);
    }

    [[nodiscard]] auto getAzimuthWaterfall() const -> const mv::gl::Waterfall<float> &
    {
        return azimuthWaterfall;
    }

    [[nodiscard]] auto getPowerWaterfall() const -> const mv::gl::Waterfall<std::uint32_t> &
    {
        return powerWaterfall;
    }

    auto pushCommand(std::function<void()> command) -> void
    {
        const std::scoped_lock lock{updateMutex};
        commandsPipe.emplace_back(std::move(command));
    }

    auto waitForFlag() -> void
    {
        continueFlag.wait(false);
        continueFlag.clear();
    }
};

#endif /* WATERFALL_HPP */
