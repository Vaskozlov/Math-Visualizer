#ifndef MV_WATERFALL_HPP
#define MV_WATERFALL_HPP

#include <ccl/text/text_iterator.hpp>
#include <glm/glm.hpp>
#include <isl/float16.hpp>
#include <isl/range.hpp>
#include <mv/color.hpp>
#include <mv/gl/texture.hpp>
#include <mv/rect.hpp>

#if defined(__AVX2__) && defined(__F16C__)
#    include <immintrin.h>
#endif

#if defined(__ARM_NEON)
#    include <arm_neon.h>
#endif

namespace mv::gl
{
    namespace detail
    {
#if defined(__AVX2__) && defined(__F16C__)
        static constexpr std::size_t vecSize = 8;

        inline auto cvtFp32ToFp16(const float *input, auto *out) -> void
        {
            const auto vec32 = _mm256_loadu_ps(input);

            const auto vec16 =
                _mm256_cvtps_ph(vec32, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);

            _mm_storeu_si128(reinterpret_cast<__m128i *>(out), vec16);
        }
#elif defined(__ARM_NEON)
        static constexpr std::size_t vecSize = 4;

        inline auto cvtFp32ToFp16(const float *value, auto *out) -> void
        {
            const auto vec32 = vld1q_f32(value);
            const auto vec16 = vcvt_f16_f32(vec32);

            // NOLINTNEXTLINE
            vst1_f16(reinterpret_cast<float16_t *>(out), vec16);
        }
#endif

        template <typename T, TextureMode Mode>
        class WaterfallBase
        {
        protected:
            std::vector<T> pixels;
            Texture texture;

        public:
            WaterfallBase(const int width, const int height, const T color)
              : pixels(static_cast<std::size_t>(width * height), color)
              , texture{
                    pixels.data(),
                    width,
                    height,
                    TextureWrapMode::CLAMP_TO_BORDER,
                    Mode,
                    TextureScaleFormat::NEAREST,
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

            auto setPixelValue(const std::size_t x, const std::size_t y, const T &value) -> void
            {
                if (x > texture.getWidth() || y > texture.getHeight()) {
                    return;
                }

                pixels[y * texture.getWidth() + x] = value;
            }

            auto reload() const -> void
            {
                texture.updateTexture();
            }

            auto bind(const int texture_number = 0) const -> void
            {
                texture.bind(texture_number);
            }

            static auto unbind() -> void
            {
                Texture::unbind();
            }

            auto resize(const std::size_t new_width, const std::size_t new_height, const T color)
                -> void
            {
                if (new_width == texture.getWidth()) {
                    pixels.resize(new_width * new_height, color);
                    texture.resize(pixels.data(), new_width, new_height);
                    return;
                }

                std::vector<T> new_pixels(new_width * new_height, color);

                for (std::size_t y = 0; y < std::min(texture.getHeight(), new_height); ++y) {
                    for (std::size_t x = 0; x < std::min(texture.getWidth(), new_width); ++x) {
                        new_pixels[y * new_width + x] = pixels[y * texture.getWidth() + x];
                    }
                }

                pixels = std::move(new_pixels);
                texture.resize(pixels.data(), new_width, new_height);
            }

            auto fill(T color) -> void
            {
                for (std::size_t i = 0; i < texture.getWidth() * texture.getHeight(); ++i) {
                    pixels[i] = color;
                }

                reload();
            }

            auto fillRect(const Rect &rect, T color) -> void
            {
                const auto start_x = std::clamp<std::size_t>(rect.x, 0, getWidth());
                const auto start_y = std::clamp<std::size_t>(rect.y, 0, getHeight());
                const auto end_x = std::clamp<std::size_t>(rect.x + rect.width, 0, getWidth());
                const auto end_y = std::clamp<std::size_t>(rect.y + rect.height, 0, getHeight());

                for (std::size_t i = start_y; i < end_y; ++i) {
                    for (std::size_t j = start_x; j < end_x; ++j) {
                        setPixelValue(j, i, color);
                    }
                }
            }

            auto drawRectangleBorder(const Rect &rect, T color, const std::uint16_t line_thickness)
                -> void
            {
                const auto line_left_offset = static_cast<isl::ssize_t>(
                    std::floor(static_cast<float>(line_thickness) / 2.0f));

                const auto line_right_offset =
                    static_cast<isl::ssize_t>(std::ceil(static_cast<float>(line_thickness) / 2.0f));

                const auto start_y = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
                    static_cast<isl::ssize_t>(rect.y) - line_left_offset,
                    0,
                    static_cast<isl::ssize_t>(getHeight())));

                const auto end_y = std::clamp<std::size_t>(
                    rect.y + rect.height + line_right_offset - 1, 0, getHeight());

                for (std::size_t i = start_y; i < end_y; ++i) {
                    const auto start_x = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
                        static_cast<isl::ssize_t>(rect.x) - line_left_offset,
                        0,
                        static_cast<isl::ssize_t>(getWidth())));

                    const auto end_x =
                        std::clamp<std::size_t>(rect.x + line_right_offset, 0, getWidth());

                    for (std::size_t image_x = start_x; image_x < end_x; ++image_x) {
                        pixels[i * texture.getWidth() + image_x] = color;

                        const auto right_x = image_x + rect.width - 1;

                        if (right_x >= getWidth()) {
                            continue;
                        }

                        pixels[i * texture.getWidth() + right_x] = color;
                    }
                }

                for (std::size_t i = rect.x; i < rect.x + rect.width; ++i) {
                    if (i >= getWidth()) {
                        continue;
                    }

                    const auto loop_start_y = static_cast<std::size_t>(std::clamp<isl::ssize_t>(
                        static_cast<isl::ssize_t>(rect.y) - line_left_offset,
                        0,
                        static_cast<isl::ssize_t>(getHeight())));

                    const auto loop_end_y = rect.y + line_right_offset;

                    for (std::size_t image_y = loop_start_y; image_y < loop_end_y; ++image_y) {
                        pixels[image_y * texture.getWidth() + i] = color;

                        const auto right_y = image_y + rect.height - 1;

                        if (right_y >= getHeight()) {
                            continue;
                        }

                        pixels[right_y * texture.getWidth() + i] = color;
                    }
                }
            }
        };
    } // namespace detail

    template <typename T>
    class Waterfall;

    template <>
    class Waterfall<RGBA<std::uint8_t>>
      : public detail::WaterfallBase<RGBA<std::uint8_t>, TextureMode::RGBA>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template <>
    class Waterfall<std::int32_t> : public detail::WaterfallBase<std::int32_t, TextureMode::I32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template <>
    class Waterfall<std::uint32_t> : public detail::WaterfallBase<std::uint32_t, TextureMode::U32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template <>
    class Waterfall<float> : public detail::WaterfallBase<float, TextureMode::F32>
    {
    public:
        using WaterfallBase::WaterfallBase;
    };

    template <>
    class Waterfall<isl::float16> : public detail::WaterfallBase<isl::float16, TextureMode::F16>
    {
    public:
        using WaterfallBase::WaterfallBase;

        auto fillLine(std::size_t y, const float *input) -> void
        {
            std::size_t i = 0;
            auto *out = pixels.data() + y * getWidth();

#if (defined(__AVX2__) && defined(__F16C__)) || (defined(__ARM_NEON))
            for (; i + detail::vecSize < getWidth() + 1; i += detail::vecSize) {
                detail::cvtFp32ToFp16(input + i, out + i);
            }
#endif
            for (; i < getWidth(); ++i) {
                out[i] = isl::fp32ToFp16(input[i]);
            }
        }
    };
} // namespace mv::gl

#endif /* MV_WATERFALL_HPP */
