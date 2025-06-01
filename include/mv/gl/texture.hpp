#ifndef MV_TEXTURE_HPP
#define MV_TEXTURE_HPP

#include <GL/glew.h>
#include <filesystem>
#include <isl/isl.hpp>

namespace mv::gl
{
    enum struct TextureScaleFormat : std::uint8_t
    {
        LINEAR,
        NEAREST,
    };

    enum struct TextureMode : std::uint8_t
    {
        R,
        RG,
        RGB,
        RGBA,
        I32,
        U32,
        F32,
        F16
    };

    enum struct TextureWrapMode : std::uint8_t
    {
        CLAMP_TO_BORDER,
        CLAMP_TO_EDGE,
        REPEAT,
        MIRRORED_REPEAT,
    };

    struct GlTextureFormat
    {
        GLenum format;
        GLenum internalFormat;
        GLenum type;
    };

    [[nodiscard]] TextureMode channelsToTextureMode(std::uint8_t channels);

    [[nodiscard]] auto getGlWrapModeValue(TextureWrapMode wrap_mode) -> GLint;

    [[nodiscard]] auto getGlTextureFormat(TextureMode texture_mode) -> GlTextureFormat;

    class Texture
    {
    protected:
        const void *data{};
        int width{};
        int height{};
        GLuint textureId{};
        TextureMode textureMode{};
        TextureScaleFormat scaleFormat{TextureScaleFormat::LINEAR};
        TextureWrapMode wrapMode{TextureWrapMode::CLAMP_TO_BORDER};

    public:
        Texture(
            const void *buffer, int width, int height, TextureWrapMode wrap_mode,
            TextureMode texture_mode, TextureScaleFormat scale_format = TextureScaleFormat::LINEAR);

        Texture(
            const unsigned char *buffer, int buffer_length, TextureWrapMode wrap_mode,
            TextureScaleFormat scale_format = TextureScaleFormat::LINEAR);

        explicit Texture(
            const std::filesystem::path &path, TextureWrapMode wrap_mode,
            TextureScaleFormat scale_format = TextureScaleFormat::LINEAR);

        ~Texture();

        auto resize(const void *buffer, const std::size_t new_width, const std::size_t new_height)
            -> void
        {
            data = buffer;
            width = static_cast<int>(new_width);
            height = static_cast<int>(new_height);
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

        [[nodiscard]] auto getTextureMode() const -> TextureMode
        {
            return textureMode;
        }

        auto bind(const int texture_number = 0) const -> void
        {
            glActiveTexture(GL_TEXTURE0 + texture_number);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }

        static auto unbind() -> void
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        auto updateTexture() const -> void;

    private:
        auto finishTextureConstruction() -> void;
    };
} // namespace mv::gl

#endif /* MV_TEXTURE_HPP */
