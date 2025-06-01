#include <mv/gl/texture3d.hpp>

namespace mv::gl
{
    Texture3D::Texture3D(
        const void *buffer, const int width, const int height, const int depth,
        const TextureMode texture_mode, const TextureWrapMode wrap_mode,
        const TextureScaleFormat scale_format)
      : data{buffer}
      , width{width}
      , height{height}
      , depth{depth}
      , textureMode{texture_mode}
      , scaleFormat{scale_format}
      , wrapMode{wrap_mode}
    {
        glGenTextures(1, &textureId);
        updateTexture();
    }

    Texture3D::~Texture3D()
    {
        glDeleteTextures(1, &textureId);
    }

    auto Texture3D::updateTexture() const -> void
    {
        const auto wrap_mode = getGlWrapModeValue(wrapMode);
        const auto [format, internal_format, type] = getGlTextureFormat(textureMode);

        glBindTexture(GL_TEXTURE_3D, textureId);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap_mode);

        glTexParameteri(
            GL_TEXTURE_3D,
            GL_TEXTURE_MIN_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);

        glTexParameteri(
            GL_TEXTURE_3D,
            GL_TEXTURE_MAG_FILTER,
            scaleFormat == TextureScaleFormat::LINEAR ? GL_LINEAR : GL_NEAREST);

        glTexImage3D(
            GL_TEXTURE_3D, 0, internal_format, width, height, depth, 0, format, type, data);

        glGenerateMipmap(GL_TEXTURE_3D);
        unbind();
    }
} // namespace mv::gl
