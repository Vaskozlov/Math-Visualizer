#ifndef MV_GL_VBO_HPP
#define MV_GL_VBO_HPP

#include <GL/glew.h>
#include <utility>

namespace mv::gl
{
    class VBO
    {
    private:
        GLuint instanceVBO = 0;

    public:
        VBO()
        {
            glGenBuffers(1, &instanceVBO);
        }

        ~VBO()
        {
            if (instanceVBO != 0) {
                glDeleteBuffers(1, &instanceVBO);
            }
        }

        VBO(const VBO &) = delete;

        VBO(VBO &&other) noexcept
          : instanceVBO{std::exchange(other.instanceVBO, 0)}
        {}

        auto operator=(VBO &&other) noexcept -> VBO &
        {
            std::swap(instanceVBO, other.instanceVBO);
            return *this;
        }

        auto operator=(const VBO &) -> VBO & = delete;

        auto bind() const -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        }

        static auto unbind() -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }


        auto loadData(const void *data, std::size_t data_size, const GLenum mode = GL_STATIC_DRAW)
            const -> void
        {
            bind();
            glBufferData(GL_ARRAY_BUFFER, data_size, data, mode);
            unbind();
        }
    };
}// namespace mv::gl

#endif /* MV_GL_VBO_HPP */
