#include <isl/isl.hpp>
#include <mv/gl/texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace mv::gl
{
    auto channelsToTextureMode(const std::uint8_t channels) -> TextureMode
    {
        switch (channels) {
        case 1:
            return TextureMode::R;
        case 2:
            return TextureMode::RG;
        case 3:
            return TextureMode::RGB;
        case 4:
            return TextureMode::RGBA;
        default:
            isl::unreachable();
        }
    }

    auto getGlWrapModeValue(const TextureWrapMode wrap_mode) -> GLint
    {
        switch (wrap_mode) {
        default:
        case TextureWrapMode::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;

        case TextureWrapMode::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;

        case TextureWrapMode::REPEAT:
            return GL_REPEAT;

        case TextureWrapMode::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        }
    }

    auto getGlTextureFormat(const TextureMode texture_mode) -> GlTextureFormat
    {
        switch (texture_mode) {
        case TextureMode::R:
            return {
                .format = GL_RED,
                .internalFormat = GL_RED,
                .type = GL_UNSIGNED_BYTE,
            };

        case TextureMode::RG:
            return {
                .format = GL_RG,
                .internalFormat = GL_RG,
                .type = GL_UNSIGNED_BYTE,
            };

        case TextureMode::RGB:
            return {
                .format = GL_RGB,
                .internalFormat = GL_RGB,
                .type = GL_UNSIGNED_BYTE,
            };

        case TextureMode::RGBA:
            return {
                .format = GL_RGBA,
                .internalFormat = GL_RGBA,
                .type = GL_UNSIGNED_BYTE,
            };

        case TextureMode::I32:
            return {
                .format = GL_RED_INTEGER,
                .internalFormat = GL_R32I,
                .type = GL_INT,
            };

        case TextureMode::U32:
            return {
                .format = GL_RED_INTEGER,
                .internalFormat = GL_R32UI,
                .type = GL_UNSIGNED_INT,
            };

        case TextureMode::F32:
            return {
                .format = GL_RED,
                .internalFormat = GL_R32F,
                .type = GL_FLOAT,
            };

        case TextureMode::F16:
            return {
                .format = GL_RED,
                .internalFormat = GL_R16F,
                .type = GL_HALF_FLOAT,
            };

        default:
            isl::unreachable();
        }
    }

    Texture::Texture(
        const void *buffer, const int width, const int height, const TextureWrapMode wrap_mode,
        const TextureMode texture_mode, const TextureScaleFormat scale_format)
      : data(buffer)
      , width(width)
      , height(height)
      , textureMode(texture_mode)
      , scaleFormat(scale_format)
      , wrapMode(wrap_mode)
    {
        finishTextureConstruction();
    }

    Texture::Texture(
        const unsigned char *buffer, const int buffer_length, const TextureWrapMode wrap_mode,
        const TextureScaleFormat scale_format)
      : scaleFormat(scale_format)
      , wrapMode(wrap_mode)
    {
        int channels = 0;
        stbi_set_flip_vertically_on_load(1);

        auto *data_ptr =
            stbi_load_from_memory(buffer, buffer_length, &width, &height, &channels, 0);
        data = data_ptr;
        textureMode = channelsToTextureMode(channels);

        finishTextureConstruction();

        stbi_image_free(data_ptr);
        data = nullptr;
    }

    Texture::Texture(
        const std::filesystem::path &path, const TextureWrapMode wrap_mode,
        const TextureScaleFormat scale_format)
      : scaleFormat(scale_format)
      , wrapMode(wrap_mode)
    {
        int channels = 0;
        stbi_set_flip_vertically_on_load(1);

        auto *data_ptr = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
        data = data_ptr;
        textureMode = channelsToTextureMode(channels);

        finishTextureConstruction();

        stbi_image_free(data_ptr);
        data = nullptr;
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &textureId);
    }

    auto Texture::finishTextureConstruction() -> void
    {
        glGenTextures(1, &textureId);
        updateTexture();
    }

    auto Texture::updateTexture() const -> void
    {
        const auto wrap_mode = getGlWrapModeValue(wrapMode);
        const auto [format, internal_format, type] = getGlTextureFormat(textureMode);

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);

        glTexParameteri(
            GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR : GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            width,
            height,
            0,
            format,
            type,
            static_cast<const void *>(data));

        glGenerateMipmap(GL_TEXTURE_2D);
        unbind();
    }

} // namespace mv::gl
