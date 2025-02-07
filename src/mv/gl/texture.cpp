#include <isl/isl.hpp>
#include <mv/gl/texture.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace mv::gl
{
    Texture::Texture(const unsigned char *buffer, const int buffer_length)
    {
        stbi_set_flip_vertically_on_load(1);
        data = stbi_load_from_memory(buffer, buffer_length, &width, &height, &channels, 0);
        finishTextureConstruction();
    }

    Texture::Texture(const std::filesystem::path &path)
    {
        stbi_set_flip_vertically_on_load(1);
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        finishTextureConstruction();
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &textureId);
    }

    auto Texture::finishTextureConstruction() -> void
    {
        GLenum format;

        switch (channels) {
        case 1:
            format = GL_RED;
            break;

        case 2:
            format = GL_RG;
            break;

        case 3:
            format = GL_RGB;
            break;

        case 4:
            format = GL_RGBA;
            break;

        default:
            isl::unreachable();
        }

        glGenTextures(1, &textureId);
        bind();

        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(
            GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
            static_cast<const void *>(data));

        glGenerateMipmap(GL_TEXTURE_2D);

        unbind();
        stbi_image_free(data);
    }
}// namespace mv::gl