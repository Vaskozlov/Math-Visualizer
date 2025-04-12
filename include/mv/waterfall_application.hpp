#ifndef MV_APPLICATION_WATERFALL_HPP
#define MV_APPLICATION_WATERFALL_HPP

#include <atomic>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/shared_lib_loader.hpp>
#include <mv/application_2d.hpp>
#include <mv/gl/shape/rectangle.hpp>
#include <mv/gl/texture.hpp>
#include <mv/gl/waterfall_application.hpp>
#include <mv/rect.hpp>
#include <mv/shader.hpp>
#include <random>

namespace mv
{
    class Waterfall : public Application2D
    {
    private:
        static constexpr std::size_t imageSize = 512;

        std::array<char, 128> imguiWindowBufferTitle{};
        std::vector<std::function<void()>> commandsPipe;

        Shader *waterfallShaderHsvF32 = getWaterfallShaderHsvF32();
        Shader *waterfallShaderLinearF32 = getWaterfallShaderLinearF32();

        gl::Waterfall<float> powerWaterfall{imageSize, imageSize};
        gl::Waterfall<RGBA<std::uint8_t>> powerWaterfallMask{imageSize, imageSize};

        gl::Waterfall<float> azimuthWaterfall{imageSize, imageSize};
        gl::Waterfall<RGBA<std::uint8_t>> azimuthWaterfallMask{imageSize, imageSize};

        std::string libraryPath;

        double pressTime = 0.0;
        ImFont *font;
        float fontScale = 0.33F;

        bool disableInput = false;

        gl::shape::Rectangle powerMapSize{0.0F, 0.0F, 1.0F, 1.0F};
        gl::shape::Rectangle azimuthMapSize{0.0F, 0.0F, 1.0F, 1.0F};

        isl::SharedLibLoader shaderLib;

        float imageWidthScale = 1.0F;
        float imageHeightScale = 1.0F;

        float azimuthMiddle = 180.0F;
        float azimuthSide = 180.0F;

        float powerLow = -20.0F;
        float powerHigh = 100.0F;

        std::mutex updateMutex;

        std::atomic_flag continueFlag{false};

        static auto getWaterfallShaderHsvF32() -> Shader *;
        static auto getWaterfallShaderLinearF32() -> Shader *;

    public:
        static constexpr float minWidthScale = 1.0F;
        static constexpr float minHeightScale = 1.0F;

        static constexpr float maxWidthScale = 20.0F;
        static constexpr float maxHeightScale = 20.0F;

        static constexpr float azimuthMin = 0.0F;
        static constexpr float azimuthMax = 360.0F;

        static constexpr float minPower = -20.0F;
        static constexpr float maxPower = 100.0F;

        static constexpr RGBA<uint8_t> white{255, 255, 255, 255};

        using Application2D::Application2D;

        auto updateAzimuthUniform() const -> void
        {
            waterfallShaderHsvF32->use();
            waterfallShaderHsvF32->setVec2(
                "minMaxValue", glm::vec2{azimuthMiddle - azimuthSide, azimuthMiddle + azimuthSide});
        }

        auto updatePowerUniform() const -> void
        {
            waterfallShaderLinearF32->use();
            waterfallShaderLinearF32->setVec2("minMaxValue", glm::vec2{powerLow, powerHigh});
        }

        auto init() -> void override;

        auto pollTask() -> void
        {
            const std::scoped_lock lock{updateMutex};

            while (!commandsPipe.empty()) {
                commandsPipe.back()();
                commandsPipe.pop_back();
            }
        }

        auto update() -> void override;

        auto processInput() -> void override;

        auto onMouseRelativeMovement(double delta_x, double delta_y) -> void override;

        auto changeWidthScale(float scale_difference) -> void;

        auto changeHeightScale(float scale_difference) -> void;

        auto onScroll(double x_offset, double y_offset) -> void override;

        auto resizeImages(const std::size_t width, const std::size_t height) -> void
        {
            azimuthWaterfall.resize(width, height);
            powerWaterfall.resize(width, height);
            powerWaterfallMask.resize(width, height);
            azimuthWaterfallMask.resize(width, height);
        }

        auto drawRect(const Rect &rect, const std::size_t line_thickness = 2) const -> void
        {
            azimuthWaterfallMask.drawRectangleBorder(rect, white, line_thickness);
            powerWaterfallMask.drawRectangleBorder(rect, white, line_thickness);
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

        [[nodiscard]] auto getAzimuthWaterfall() const -> const gl::Waterfall<float> &
        {
            return azimuthWaterfall;
        }

        [[nodiscard]] auto getPowerWaterfall() const -> const gl::Waterfall<float> &
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
} // namespace mv

#endif /* MV_APPLICATION_WATERFALL_HPP */
