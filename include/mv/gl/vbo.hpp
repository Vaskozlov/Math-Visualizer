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
        VBO();

        ~VBO();

        VBO(const VBO &) = delete;
        VBO(VBO &&other) noexcept;

        auto operator=(VBO &&other) noexcept -> VBO &;
        auto operator=(const VBO &) -> VBO & = delete;

        auto bind() const -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        }

        static auto unbind() -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        auto loadData(const void *data, std::size_t data_size, GLenum mode = GL_STATIC_DRAW) const
            -> void;
    };
} // namespace mv::gl

#endif /* MV_GL_VBO_HPP */
