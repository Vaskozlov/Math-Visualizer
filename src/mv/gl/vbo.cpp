#include <mv/gl/vbo.hpp>

namespace mv::gl
{
    VBO::VBO()
    {
        glGenBuffers(1, &instanceVBO);
    }

    VBO::~VBO()
    {
        if (instanceVBO != 0) {
            glDeleteBuffers(1, &instanceVBO);
        }
    }

    VBO::VBO(VBO &&other) noexcept
      : instanceVBO{std::exchange(other.instanceVBO, 0)}
    {}

    auto VBO::operator=(VBO &&other) noexcept -> VBO &
    {
        std::swap(instanceVBO, other.instanceVBO);
        return *this;
    }

    auto VBO::loadData(const void *data, const std::size_t data_size, const GLenum mode) const
        -> void
    {
        bind();
        glBufferData(GL_ARRAY_BUFFER, data_size, data, mode);
        unbind();
    }
} // namespace mv::gl
