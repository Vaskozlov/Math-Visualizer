#ifndef MV_TEXTURE_HPP
#define MV_TEXTURE_HPP

#include <battery/embed.hpp>
#include <filesystem>
#include <GL/glew.h>

namespace mv::gl
{
    class Texture
    {
    protected:
        const unsigned char *data{};
        int width{};
        int height{};
        int channels{};
        GLuint textureId{};

    public:
        Texture(const unsigned char *buffer, int width, int height, int channels);

        Texture(const unsigned char *buffer, int buffer_length);

        explicit Texture(const std::filesystem::path &path);

        ~Texture();

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

        [[nodiscard]] auto getChannels() const -> std::size_t
        {
            return static_cast<std::size_t>(channels);
        }

        auto bind() const -> void
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }

        static auto unbind() -> void
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        template<b::embed_string_literal fontPath>
        [[nodiscard]] static auto loadEmbeddedTexture() -> Texture
        {
            const auto *font_data = static_cast<const unsigned char *>(b::embed<fontPath>().data());
            const int font_data_size = static_cast<int>(b::embed<fontPath>().size());

            return {font_data, font_data_size};
        }

        auto updateTexture() const -> void;

    private:
        auto finishTextureConstruction() -> void;
    };
}// namespace mv::gl

#endif /* MV_TEXTURE_HPP */
