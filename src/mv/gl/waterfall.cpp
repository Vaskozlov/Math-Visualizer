#define GLM_ENABLE_EXPERIMENTAL

#include <mv/gl/waterfall.hpp>

namespace mv::gl
{
    Waterfall::Waterfall(const int width, const int height)
      : pixels(std::make_unique<RGBA<std::uint8_t>[]>(static_cast<std::size_t>(width * height)))
      , texture(reinterpret_cast<unsigned char *>(pixels.get()), width, height, 4)
    {}

    auto Waterfall::setPixelValue(
        const std::size_t x, const std::size_t y, const isl::ssize_t value) const -> void
    {
        pixels[y * texture.getWidth() + x] = intToRainbowColor(value, rangeStart, rangeEnd);
    }

    auto Waterfall::reload() const -> void
    {
        texture.updateTexture();
    }
}// namespace mv::gl