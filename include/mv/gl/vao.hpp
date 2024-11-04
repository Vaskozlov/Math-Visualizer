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
        VAO()
        {
            glGenVertexArrays(1, &instanceVAO);
        }

        ~VAO()
        {
            if (instanceVAO != 0) {
                glDeleteVertexArrays(1, &instanceVAO);
            }
        }

        VAO(const VAO &) = delete;

        VAO(VAO &&other) noexcept
          : instanceVAO{std::exchange(other.instanceVAO, 0)}
        {}

        auto operator=(VAO &&other) noexcept -> VAO &
        {
            std::swap(instanceVAO, other.instanceVAO);
            return *this;
        }

        auto operator=(const VAO &) -> VAO & = delete;

        auto bind() const -> void
        {
            glBindVertexArray(instanceVAO);
        }

        static auto unbind() -> void
        {
            glBindVertexArray(0);
        }

        auto bind(
            const GLsizei array_attribute, const GLint size, const GLenum type,
            const GLsizei stride, const GLsizei offset) const -> void
        {
            bind();
            bindAttribute(array_attribute, size, type, stride, offset);
            unbind();
        }

        auto bindInstanceParameters(const GLsizei array_attribute, const GLint divisor) const -> void
        {
            bind();

            for (GLsizei i = 0; i < sizeof(InstanceParameters) / sizeof(glm::vec4); ++i) {
                bindAttribute(
                    array_attribute + i, 4, GL_FLOAT, sizeof(InstanceParameters),
                    sizeof(glm::vec4) * i); // NOLINT
                glVertexAttribDivisor(array_attribute + i, divisor);
            }

            unbind();
        }

    private:
        static auto bindAttribute(
            const GLsizei array_attribute, const GLint size, const GLenum type,
            const GLsizei stride, const GLsizei offset) -> void
        {
            glVertexAttribPointer(
                array_attribute, size, type, GL_FALSE, stride,
                static_cast<const void *>(static_cast<const char *>(0x0) + offset));// NOLINT

            glEnableVertexAttribArray(array_attribute);
        }
    };
}// namespace mv::gl

#endif /* MV_GL_VAO_HPP */
