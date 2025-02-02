#ifndef MV_COLOR_HPP
#define MV_COLOR_HPP

#include <glm/glm.hpp>

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
                    std::max(r * scale, 1.0F),
                    std::max(g * scale, 1.0F),
                    std::max(b * scale, 1.0F),
                    a,
                };
            }
        };
    }// namespace detail

    struct Color : detail::ColorImpl
    {
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
}// namespace mv

#endif /* MV_COLOR_HPP */
