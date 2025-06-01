#ifndef MV_GL_TEXTURE3D_HPP
#define MV_GL_TEXTURE3D_HPP

#include <mv/gl/texture.hpp>

namespace mv::gl
{
    class Texture3D
    {
    protected:
        const void *data{};
        int width{};
        int height{};
        int depth{};
        GLuint textureId{};
        TextureMode textureMode{};
        TextureScaleFormat scaleFormat{TextureScaleFormat::LINEAR};
        TextureWrapMode wrapMode{TextureWrapMode::CLAMP_TO_BORDER};

    public:
        Texture3D(
            const void *buffer, int width, int height, int depth, TextureMode texture_mode,
            TextureWrapMode wrap_mode = TextureWrapMode::CLAMP_TO_BORDER,
            TextureScaleFormat scale_format = TextureScaleFormat::LINEAR);

        ~Texture3D();

        auto resize(
            const void *buffer, const std::size_t new_width, const std::size_t new_height,
            const std::size_t new_depth) -> void
        {
            data = buffer;
            width = static_cast<int>(new_width);
            height = static_cast<int>(new_height);
            depth = static_cast<int>(new_depth);

            updateTexture();
        }

        [[nodiscard]] auto getTextureId() const -> GLuint
        {
            return textureId;
        }

        [[nodiscard]] auto getWidth() const -> std::size_t
        {
            return static_cast<std::size_t>(width);
        }

        [[nodiscard]] auto getHeight() const -> std::size_t
        {
            return static_cast<std::size_t>(height);
        }

        [[nodiscard]] auto getDepth() const -> std::size_t
        {
            return static_cast<std::size_t>(depth);
        }

        [[nodiscard]] auto getScaleFormat() const -> TextureScaleFormat
        {
            return scaleFormat;
        }

        [[nodiscard]] auto getWrapMode() const -> TextureWrapMode
        {
            return wrapMode;
        }

        auto bind(const int texture_number = 0) const -> void
        {
            glActiveTexture(GL_TEXTURE0 + texture_number);
            glBindTexture(GL_TEXTURE_3D, textureId);
        }

        static auto unbind() -> void
        {
            glBindTexture(GL_TEXTURE_3D, 0);
        }

        auto updateTexture() const -> void;
    };
} // namespace mv::gl

#endif /* MV_GL_TEXTURE3D_HPP */
