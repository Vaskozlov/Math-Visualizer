#ifndef MV_APPLICATION_WATERFALL_HPP
#define MV_APPLICATION_WATERFALL_HPP

#include "gl/instances_holder.hpp"
#include "gl/shaders/color_shader.hpp"
#include "gl/shaders/shader_with_positioning.hpp"

#include <atomic>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/shared_lib_loader.hpp>
#include <list>
#include <mv/application_2d.hpp>
#include <mv/gl/shape/rectangle.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/texture.hpp>
#include <mv/gl/waterfall.hpp>
#include <mv/rect.hpp>
#include <mv/shader.hpp>
#include <numbers>
#include <random>

namespace mv
{
    struct RectWithAzimuthAndPower : Rect
    {
        float azimuth{};
        float power{};
    };

    struct RectangleInstance
    {
        glm::vec2 color;
        glm::mat4 model;
    };

    class Waterfall : public Application2D
    {
    protected:
        static constexpr std::size_t imageSize = 512;

        std::array<char, 128> imguiWindowBufferTitle{};
        std::vector<std::function<void()>> commandsPipe;

        Shader *waterfallShaderHsvF32 = getWaterfallShaderHsvF32();
        Shader *waterfallShaderLinearF32 = getWaterfallShaderLinearF32();
        Shader *colorShader = gl::getShaderWithPositioning();
        Shader *shaderHsvWithModel = gl::getHsvShaderWithModel();
        Shader *shaderLinearWithModel = gl::getLinearShaderWithModel();

        std::list<gl::Waterfall<gl::float16>> powerWaterfalls;
        std::list<gl::Waterfall<gl::float16>> azimuthWaterfalls;

        gl::Waterfall<RGBA<std::uint8_t>> powerWaterfallMask{imageSize, imageSize};
        gl::Waterfall<RGBA<std::uint8_t>> azimuthWaterfallMask{imageSize, imageSize};
        gl::InstancesHolder<gl::InstanceParameters> rectangleInstances;
        gl::shape::Rectangle rectangle{0.0F, 0.0F, 1.0F, 1.0F};
        gl::shape::Sphere sphere{0.1F};

        double pressTime = 0.0;
        ImFont *font;
        float fontScale = 0.5F;

        bool disableInput = false;

        gl::shape::Rectangle powerMapSize{0.0F, 0.0F, 1.0F, 1.0F};
        gl::shape::Rectangle azimuthMapSize{0.0F, 0.0F, 1.0F, 1.0F};

        float imageWidthScale = 0.9F;
        float imageHeightScale = 0.9F;

        float azimuthMiddle = 180.0F;
        float azimuthSide = 180.0F;

        float powerLow = -20.0F;
        float powerHigh = 100.0F;

        float frequencyPosition = 0.0F;

        std::size_t maxTextureSize;
        std::mutex updateMutex;

        std::atomic_flag continueFlag{false};

        std::size_t waterfallWidth{imageSize};
        std::size_t waterfallHeight{imageSize};

        std::vector<RectWithAzimuthAndPower> detections;

        bool showAzimuthPoints{true};
        bool showPowerPoints{};
        bool showDetectionAzimuth{true};
        bool showDetectionPower{};

        static auto getWaterfallShaderHsvF32() -> Shader *;
        static auto getWaterfallShaderLinearF32() -> Shader *;

    public:
        static constexpr float minWidthScale = 0.9F;
        static constexpr float minHeightScale = 0.9F;

        static constexpr float maxWidthScale = 500.0F;
        static constexpr float maxHeightScale = 10.0F;

        static constexpr float azimuthMin = 0.0F;
        static constexpr float azimuthMax = 360.0F;

        static constexpr float minPower = -20.0F;
        static constexpr float maxPower = 100.0F;

        double frequencyScale = 1.0F;
        double timeScale = 1.0F;

        static constexpr RGBA<uint8_t> white{255, 255, 255, 255};

        using Application2D::Application2D;

        auto updateAzimuthUniform() const -> void
        {
            waterfallShaderHsvF32->use();
            waterfallShaderHsvF32->setVec2(
                "minMaxValue",
                glm::vec2{
                    azimuthMiddle - azimuthSide,
                    azimuthMiddle + azimuthSide,
                });

            shaderHsvWithModel->use();
            shaderHsvWithModel->setVec2(
                "minMaxValue",
                glm::vec2{
                    azimuthMiddle - azimuthSide,
                    azimuthMiddle + azimuthSide,
                });
        }

        auto updatePowerUniform() const -> void
        {
            waterfallShaderLinearF32->use();
            waterfallShaderLinearF32->setVec2(
                "minMaxValue",
                glm::vec2{
                    powerLow,
                    powerHigh,
                });

            shaderLinearWithModel->use();
            shaderLinearWithModel->setVec2(
                "minMaxValue",
                glm::vec2{
                    powerLow,
                    powerHigh,
                });
        }

        auto clearDetections() -> void
        {
            detections.clear();
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

        auto resizeImages(std::size_t width, std::size_t height) -> void;

        auto addRect(const RectWithAzimuthAndPower &detection) -> void
        {
            detections.emplace_back(detection);
        }

        auto fill(float value) const -> void;

        auto reloadImages() const -> void;

        auto setPixel(std::size_t x, std::size_t y, gl::float16 azimuth, gl::float16 power) const
            -> void;

        [[nodiscard]] auto getAzimuthWaterfalls() const
            -> const std::list<gl::Waterfall<gl::float16>> &
        {
            return azimuthWaterfalls;
        }

        [[nodiscard]] auto getPowerWaterfalls() const
            -> const std::list<gl::Waterfall<gl::float16>> &
        {
            return powerWaterfalls;
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

        auto drawDetections() -> void;

    private:
        auto drawAzimuthWaterfalls(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawPowerWaterfalls(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawAzimuthDetections(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawPowerDetections(const glm::mat4 &projection, float offset_width_scale) const -> void;
    };
} // namespace mv

#endif /* MV_APPLICATION_WATERFALL_HPP */
