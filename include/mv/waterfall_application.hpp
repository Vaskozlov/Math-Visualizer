#ifndef MV_APPLICATION_WATERFALL_HPP
#define MV_APPLICATION_WATERFALL_HPP

#include <atomic>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/shared_lib_loader.hpp>
#include <list>
#include <mv/application_2d.hpp>
#include <mv/gl/instances_holder.hpp>
#include <mv/gl/shaders/color_shader.hpp>
#include <mv/gl/shaders/shader_with_positioning.hpp>
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
    private:
        std::array<char, 128> imguiWindowBufferTitle{};
        std::array<char, 64> timeFormattingBuffer1{};
        std::array<char, 64> timeFormattingBuffer2{};

    protected:
        Shader *waterfallShaderHsvF32 = getWaterfallShaderHsvF32();
        Shader *waterfallShaderLinearF32 = getWaterfallShaderLinearF32();
        Shader *colorShader = gl::getShaderWithPositioning();
        Shader *shaderHsvWithModel = gl::getHsvShaderWithModel();
        Shader *shaderLinearWithModel = gl::getLinearShaderWithModel();

        std::list<gl::Waterfall<isl::float16>> powerWaterfalls;
        std::list<gl::Waterfall<isl::float16>> azimuthWaterfalls;

        gl::InstancesHolder<gl::InstanceParameters> rectangleInstances;
        gl::shape::Rectangle rectangle{0.0F, 0.0F, 1.0F, 1.0F};
        gl::shape::Sphere sphere{0.1F};

        double pressTime = 0.0;
        ImFont *font;
        float fontScale = 0.5F;

        bool disableInput = false;

        gl::shape::Rectangle powerMapSize{0.0F, 0.0F, 1.0F, 1.0F};
        gl::shape::Rectangle azimuthMapSize{0.0F, 0.0F, 1.0F, 1.0F};

        float imageWidthScale = 1.0F;
        float imageHeightScale = 1.1F;

        float azimuthMiddle = 180.0F;
        float azimuthSide = 180.0F;

        float powerLow = -20.0F;
        float powerHigh = 100.0F;

        float frequencyPosition = 0.0F;
        float timePosition = 0.0;

        float mouseWordX = 0.0F;
        float mouseWordY = 0.0F;

        float mouseX = 0.0F;
        float mouseY = 0.0F;

        std::size_t maxTextureSize;
        std::mutex updateMutex;

        std::atomic_flag continueFlag = ATOMIC_FLAG_INIT;

        std::size_t waterfallWidth{};
        std::size_t waterfallHeight{};

        std::vector<RectWithAzimuthAndPower> detections;

        glm::vec2 waterfallStart{-0.9F, -0.9F};

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
        double frequencyStartOffset = 0.0;
        double timeScale = 1.0F;
        double timeStartOffset = 0.0;

        static constexpr RGBA<uint8_t> white{255, 255, 255, 255};

        using Application2D::Application2D;

        ~Waterfall() override;

        auto clearDetections() -> void
        {
            detections.clear();
        }

        auto init() -> void override;

        auto update() -> void override;

        auto processInput() -> void override;

        auto onMouseMovement(double x_pos_in, double y_pos_in) -> void override;

        auto onMouseRelativeMovement(double delta_x, double delta_y) -> void override;

        auto changeWidthScale(float scale_difference) -> void;

        auto changeHeightScale(float scale_difference) -> void;

        auto onScroll(double x_offset, double y_offset) -> void override;

        [[nodiscard]] auto getCameraProjection() const -> glm::mat4 override;

        auto resizeImages(
            std::size_t width, std::size_t height, isl::float16 default_azimuth,
            isl::float16 default_power) -> isl::AsyncTask<void>;

        auto resizeToFit(
            double max_frequency, std::size_t max_time, isl::float16 default_azimuth,
            isl::float16 default_power) -> isl::AsyncTask<void>;

        auto clear() -> isl::AsyncTask<void>
        {
            return submit(doClear());
        }

        auto addRect(const RectWithAzimuthAndPower &detection) -> void
        {
            detections.emplace_back(detection);
        }

        auto fill(float value) -> isl::AsyncTask<void>;

        auto reloadImages() -> isl::AsyncTask<void>;

        auto setPixel(std::size_t x, std::size_t y, isl::float16 azimuth, isl::float16 power)
            -> void;

        auto setPixelAzimuth(std::size_t x, std::size_t y, isl::float16 azimuth) -> void;

        auto setPixelPower(std::size_t x, std::size_t y, isl::float16 power) -> void;

        [[nodiscard]] auto getAzimuthWaterfalls() const
            -> const std::list<gl::Waterfall<isl::float16>> &
        {
            return azimuthWaterfalls;
        }

        [[nodiscard]] auto getPowerWaterfalls() const
            -> const std::list<gl::Waterfall<isl::float16>> &
        {
            return powerWaterfalls;
        }

        auto waitForFlag() -> bool
        {
            continueFlag.wait(false);

            if (!continueFlag.test()) {
                return false;
            }

            continueFlag.clear();
            return true;
        }

    private:
        auto doFill(float value) -> isl::Task<>;

        auto updateAzimuthUniform() const -> void;

        auto updatePowerUniform() const -> void;

        auto doClear() -> isl::Task<>;

        auto drawDetections() -> void;

        auto doReloadImages() const -> isl::Task<>;

        auto doResizeImages(
            std::size_t width, std::size_t height, isl::float16 default_azimuth,
            isl::float16 default_power) -> isl::Task<>;

        auto drawAzimuthWaterfalls(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawPowerWaterfalls(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawAzimuthDetections(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        auto drawPowerDetections(const glm::mat4 &projection, float offset_width_scale) const
            -> void;

        [[nodiscard]] auto getPointOn2DScene() const -> glm::vec2;
    };
} // namespace mv

#endif /* MV_APPLICATION_WATERFALL_HPP */
