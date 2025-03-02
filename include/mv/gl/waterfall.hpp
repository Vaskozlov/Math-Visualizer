#ifndef MV_WATERFALL_HPP
#define MV_WATERFALL_HPP

#include <ccl/text/text_iterator.hpp>
#include <isl/range.hpp>
#include <mv/color.hpp>
#include <mv/gl/texture.hpp>

namespace mv::gl
{
    namespace detail
    {
        template<typename T, TextureMode Mode>
        class WaterfallBase
        {
        protected:
            std::unique_ptr<T[]> pixels{};// NOLINT
            Texture texture;

        public:
            WaterfallBase(const int width, const int height)
              : pixels(std::make_unique<T[]>(static_cast<std::size_t>(width * height)))// NOLINT
              , texture{
                    pixels.get(), width,
                    height,       TextureWrapMode::CLAMP_TO_BORDER,
                    Mode,         TextureScaleFormat::NEAREST,
                }
            {}

            [[nodiscard]] auto getWidth() const -> std::size_t
            {
                return texture.getWidth();
            }

            [[nodiscard]] auto getHeight() const -> std::size_t
            {
                return texture.getHeight();
            }

            [[nodiscard]] auto getTextureMode() const -> TextureMode
            {
                return texture.getTextureMode();
            }

            [[nodiscard]] auto getTextureId() const -> GLuint
            {
                return texture.getTextureId();
            }

            auto setPixelValue(const std::size_t x, const std::size_t y, const T &value) const
                -> void
            {
                pixels[y * texture.getWidth() + x] = value;
            }

            auto reload() const -> void
            {
                texture.updateTexture();
            }

            auto bind(const int texture_number = GL_TEXTURE0) const -> void
            {
                texture.bind(texture_number);
            }

            static auto unbind() -> void
            {
                Texture::unbind();
            }

            auto resize(const std::size_t new_width, const std::size_t new_height) -> void
            {
                std::unique_ptr<T[]> new_pixels = std::make_unique<T[]>(
                    static_cast<std::size_t>(new_width * new_height));// NOLINT

                for (std::size_t y = 0; y < texture.getHeight(); ++y) {
                    for (std::size_t x = 0; x < texture.getWidth(); ++x) {
                        new_pixels[y * new_width + x] = pixels[y * texture.getWidth() + x];
                    }
                }

                pixels = std::move(new_pixels);
                texture.resize(pixels.get(), new_width, new_height);
            }
        };
    }// namespace detail

    template<typename T>
    class Waterfall;

    template<>
    class Waterfall<RGBA<std::uint8_t>>
      : public detail::WaterfallBase<RGBA<std::uint8_t>, TextureMode::RGBA>
    {
    private:
        isl::ssize_t rangeStart{};
        isl::ssize_t rangeEnd{100};

    public:
        using WaterfallBase::WaterfallBase;

        auto setRange(const isl::ssize_t from, const isl::ssize_t to) -> void
        {
            rangeStart = from;
            rangeEnd = to;
        }

        auto setPixelValue(
            std::size_t x, std::size_t y, isl::ssize_t value, std::uint8_t alpha) const -> void;

        auto drawRectangleBorder(
            std::size_t x, std::size_t y, std::size_t width, std::size_t height,
            RGBA<std::uint8_t> color, std::uint16_t line_thickness) const -> void;
    };

    template<>
    class Waterfall<std::int32_t> : public detail::WaterfallBase<std::int32_t, TextureMode::I32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template<>
    class Waterfall<std::uint32_t> : public detail::WaterfallBase<std::uint32_t, TextureMode::U32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template<>
    class Waterfall<float> : public detail::WaterfallBase<float, TextureMode::F32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };
}// namespace mv::gl

#endif /* MV_WATERFALL_HPP */
