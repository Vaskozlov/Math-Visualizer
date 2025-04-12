#ifndef MV_COLOR_HPP
#define MV_COLOR_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/color_space.hpp>
#include <isl/isl.hpp>

namespace mv
{
    namespace detail
    {
        struct ColorImpl : glm::vec4
        {
            using glm::vec4::vec4;

            ISL_DECL auto darker(const float scale = 0.5F) const noexcept -> ColorImpl
            {
                return ColorImpl{
                    r * scale,
                    g * scale,
                    b * scale,
                    a,
                };
            }

            ISL_DECL auto lighter(const float scale = 1.5F) const noexcept -> ColorImpl
            {
                return ColorImpl{
                    std::fmax(r * scale, 1.0F),
                    std::fmax(g * scale, 1.0F),
                    std::fmax(b * scale, 1.0F),
                    a,
                };
            }
        };
    } // namespace detail

    struct Color : detail::ColorImpl
    {
        using ColorImpl::ColorImpl;

        Color() = default;

        Color(const ColorImpl impl) noexcept // NOLINT
          : ColorImpl{impl}
        {}

        static constexpr ColorImpl WHITE{1.0F, 1.0F, 1.0F, 1.0F};
        static constexpr ColorImpl BLACK{0.0F, 0.0F, 0.0F, 1.0F};
        static constexpr ColorImpl RED{1.0F, 0.0F, 0.0F, 1.0F};
        static constexpr ColorImpl GREEN{0.0F, 1.0F, 0.0F, 1.0F};
        static constexpr ColorImpl BLUE{0.0F, 0.0F, 1.0F, 1.0F};
        static constexpr ColorImpl YELLOW{1.0F, 1.0F, 0.0F, 1.0F};
        static constexpr ColorImpl CYAN{0.0F, 1.0F, 1.0F, 1.0F};
        static constexpr ColorImpl MAGENTA{1.0F, 0.0F, 1.0F, 1.0F};
        static constexpr ColorImpl GRAY{0.5F, 0.5F, 0.5F, 1.0F};
        static constexpr ColorImpl LIGHT_GRAY{0.75F, 0.75F, 0.75F, 1.0F};
        static constexpr ColorImpl DARK_GRAY{0.25F, 0.25F, 0.25F, 1.0F};
        static constexpr ColorImpl TRANSPARENT{0.0F, 0.0F, 0.0F, 0.0F};
        static constexpr ColorImpl ORANGE{1.0F, 0.5F, 0.0F, 1.0F};
        static constexpr ColorImpl PINK{1.0F, 0.0F, 0.5F, 1.0F};
        static constexpr ColorImpl PURPLE{0.5F, 0.0F, 1.0F, 1.0F};
        static constexpr ColorImpl BROWN{0.5F, 0.25F, 0.0F, 1.0F};
        static constexpr ColorImpl OLIVE{0.5F, 0.5F, 0.0F, 1.0F};
        static constexpr ColorImpl TEAL{0.0F, 0.5F, 0.5F, 1.0F};
        static constexpr ColorImpl NAVY{0.0F, 0.0F, 0.5F, 1.0F};
        static constexpr ColorImpl MAROON{0.5F, 0.0F, 0.0F, 1.0F};
        static constexpr ColorImpl FOREST{0.0F, 0.5F, 0.0F, 1.0F};
        static constexpr ColorImpl AQUA{0.0F, 1.0F, 1.0F, 1.0F};
        static constexpr ColorImpl LIGHT_GREEN{0.1F, 0.8F, 0.1F, 1.0F};
        static constexpr ColorImpl DARK_ORANGE{1.0F, 0.3F, 0.0F, 1.0F};
    };

    template <typename T>
    struct RGBA
    {
        T r{};
        T g{};
        T b{};
        T a{};
    };

    inline auto hsv2rgb(const float h, const float s, const float v) -> RGBA<std::uint8_t>
    {
        const auto color = glm::rgbColor(glm::vec3{h, s, v}) * glm::vec3(255.0F);

        return RGBA<std::uint8_t>{
            .r = static_cast<uint8_t>(color.r),
            .g = static_cast<uint8_t>(color.g),
            .b = static_cast<uint8_t>(color.b),
            .a = 255,
        };
    }

    template <std::integral T>
    auto intToRainbowColor(T value, const T minValue, const T maxValue) -> RGBA<std::uint8_t>
    {
        value = std::clamp(value, minValue, maxValue);

        const float hue =
            360.0F * static_cast<float>(value - minValue) / static_cast<float>(maxValue - minValue);

        return hsv2rgb(hue, 1.0F, 1.0F);
    }
} // namespace mv

#endif /* MV_COLOR_HPP */
