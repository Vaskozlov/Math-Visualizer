#ifndef MV_GL_VAO_HPP
#define MV_GL_VAO_HPP

#include <GL/glew.h>
#include <mv/gl/instance_parameters.hpp>
#include <utility>

namespace mv::gl
{
    class VAO
    {
    private:
        GLuint instanceVAO = 0;

    public:
        VAO();

        ~VAO();

        VAO(const VAO &) = delete;

        VAO(VAO &&other) noexcept;

        auto operator=(VAO &&other) noexcept -> VAO &;

        auto operator=(const VAO &) -> VAO & = delete;

        auto bind() const -> void
        {
            glBindVertexArray(instanceVAO);
        }

        static auto unbind() -> void
        {
            glBindVertexArray(0);
        }

        auto bind(GLsizei array_attribute, GLint size, GLenum type, GLsizei stride, GLsizei offset)
            const -> void;

        auto bindInstanceParameters(GLsizei array_attribute, GLint divisor) const -> void;
    };
}// namespace mv::gl

#endif /* MV_GL_VAO_HPP */
