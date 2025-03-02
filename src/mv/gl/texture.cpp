#include <isl/isl.hpp>
#include <mv/gl/texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

namespace mv::gl
{
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

        auto *data_ptr = stbi_load(path.c_str(), &width, &height, &channels, 0);
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
        GLenum format;
        GLint internal_format;
        GLenum type;
        GLint wrap_mode;

        switch (wrapMode) {
        default:
        case TextureWrapMode::CLAMP_TO_BORDER:
            wrap_mode = GL_CLAMP_TO_BORDER;
            break;

        case TextureWrapMode::CLAMP_TO_EDGE:
            wrap_mode = GL_CLAMP_TO_EDGE;
            break;

        case TextureWrapMode::REPEAT:
            wrap_mode = GL_REPEAT;
            break;

        case TextureWrapMode::MIRRORED_REPEAT:
            wrap_mode = GL_MIRRORED_REPEAT;
            break;
        }

        switch (textureMode) {
        case TextureMode::R:
            format = GL_RED;
            internal_format = GL_RED;
            type = GL_UNSIGNED_BYTE;
            break;

        case TextureMode::RG:
            format = GL_RG;
            internal_format = GL_RG;
            type = GL_UNSIGNED_BYTE;
            break;

        case TextureMode::RGB:
            format = GL_RGB;
            internal_format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;

        case TextureMode::RGBA:
            format = GL_RGBA;
            internal_format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;

        case TextureMode::I32:
            format = GL_RED_INTEGER;
            internal_format = GL_R32I;
            type = GL_INT;
            break;

        case TextureMode::U32:
            format = GL_RED_INTEGER;
            internal_format = GL_R32UI;
            type = GL_UNSIGNED_INT;
            break;

        case TextureMode::F32:
            format = GL_RED;
            internal_format = GL_R32F;
            type = GL_FLOAT;
            break;

        default:
            isl::unreachable();
        }

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR : GL_NEAREST);

        glTexImage2D(
            GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type,
            static_cast<const void *>(data));

        glGenerateMipmap(GL_TEXTURE_2D);
        unbind();
    }

}// namespace mv::gl