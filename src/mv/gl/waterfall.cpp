#define GLM_ENABLE_EXPERIMENTAL

#include <mv/gl/waterfall.hpp>

namespace mv::gl
{
    auto Waterfall<RGBA<std::uint8_t>>::setPixelValue(
        const std::size_t x, const std::size_t y, const isl::ssize_t value,
        const std::uint8_t alpha) const -> void
    {
        pixels[y * texture.getWidth() + x] = intToRainbowColor(value, rangeStart, rangeEnd);
        pixels[y * texture.getWidth() + x].a = alpha;
    }

    auto Waterfall<RGBA<std::uint8_t>>::drawRectangleBorder(
        const std::size_t x, const std::size_t y, const std::size_t width, const std::size_t height,
        const RGBA<std::uint8_t> color, const std::uint16_t line_thickness) const -> void
    {
        const auto line_left_offset =
            static_cast<isl::ssize_t>(std::ceil(static_cast<float>(line_thickness) / 2.0f));

        const auto line_right_offset = static_cast<std::size_t>(line_left_offset);

        const auto start_y = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
            static_cast<isl::ssize_t>(y) - line_left_offset, 0,
            static_cast<isl::ssize_t>(getHeight())));

        const auto end_y =
            std::clamp<std::size_t>(y + height + line_right_offset - 1, 0, getHeight());

        for (std::size_t i = start_y; i < end_y; ++i) {
            const auto start_x = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
                static_cast<isl::ssize_t>(x) - line_left_offset, 0,
                static_cast<isl::ssize_t>(getWidth())));

            const auto end_x = std::clamp<std::size_t>(x + line_right_offset, 0, getWidth());

            for (std::size_t image_x = start_x; image_x < end_x; ++image_x) {
                pixels[i * texture.getWidth() + image_x] = color;

                const auto right_x = image_x + width - 1;

                if (right_x >= getWidth()) {
                    continue;
                }

                pixels[i * texture.getWidth() + right_x] = color;
            }
        }

        for (std::size_t i = x; i < x + width; ++i) {
            if (i >= getWidth()) {
                continue;
            }

            const auto loop_start_y = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
                static_cast<isl::ssize_t>(y) - line_left_offset, 0,
                static_cast<isl::ssize_t>(getHeight())));

            const auto loop_end_y = y + line_right_offset;

            for (std::size_t image_y = loop_start_y; image_y < loop_end_y; ++image_y) {
                pixels[image_y * texture.getWidth() + i] = color;

                const auto right_y = image_y + height - 1;

                if (right_y >= getHeight()) {
                    continue;
                }

                pixels[right_y * texture.getWidth() + i] = color;
            }
        }
    }
}// namespace mv::gl