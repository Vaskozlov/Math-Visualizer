#ifndef MV_WATERFALL_HPP
#define MV_WATERFALL_HPP

#include <isl/range.hpp>
#include <mv/color.hpp>
#include <mv/gl/texture.hpp>

namespace mv::gl
{
    class Waterfall
    {
    private:
        std::unique_ptr<RGBA<std::uint8_t>[]> pixels{};
        Texture texture;
        isl::ssize_t rangeStart{};
        isl::ssize_t rangeEnd{100};

    public:
        Waterfall(int width, int height);

        auto setRange(const isl::ssize_t from, const isl::ssize_t to) -> void
        {
            rangeStart = from;
            rangeEnd = to;
        }

        [[nodiscard]] auto getWidth() const -> std::size_t
        {
            return texture.getWidth();
        }

        [[nodiscard]] auto getHeight() const -> std::size_t
        {
            return texture.getHeight();
        }

        [[nodiscard]] auto getChannels() const -> std::size_t
        {
            return texture.getChannels();
        }

        auto setPixelValue(std::size_t x, std::size_t y, isl::ssize_t value) const -> void;

        auto reload() const -> void;

        auto bind() const -> void
        {
            texture.bind();
        }

        static auto unbind() -> void
        {
            Texture::unbind();
        }
    };
}// namespace mv::gl

#endif /* MV_WATERFALL_HPP */
