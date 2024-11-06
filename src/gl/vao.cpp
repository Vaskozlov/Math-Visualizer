#include <mv/gl/vao.hpp>

namespace mv::gl
{
    static auto bindAttribute(
        const GLsizei array_attribute, const GLint size, const GLenum type, const GLsizei stride,
        const GLsizei offset) -> void
    {
        glVertexAttribPointer(
            array_attribute, size, type, GL_FALSE, stride,
            static_cast<const void *>(static_cast<const char *>(0x0) + offset));// NOLINT

        glEnableVertexAttribArray(array_attribute);
    }

    VAO::VAO()
    {
        glGenVertexArrays(1, &instanceVAO);
    }

    VAO::~VAO()
    {
        if (instanceVAO != 0) {
            glDeleteVertexArrays(1, &instanceVAO);
        }
    }

    VAO::VAO(VAO &&other) noexcept
      : instanceVAO{std::exchange(other.instanceVAO, 0)}
    {}

    auto VAO::operator=(VAO &&other) noexcept -> VAO &
    {
        std::swap(instanceVAO, other.instanceVAO);
        return *this;
    }

    auto VAO::bind(
        const GLsizei array_attribute, const GLint size, const GLenum type, const GLsizei stride,
        const GLsizei offset) const -> void
    {
        bind();
        bindAttribute(array_attribute, size, type, stride, offset);
        unbind();
    }

    auto VAO::bindInstanceParameters(const GLsizei array_attribute, const GLint divisor) const
        -> void
    {
        bind();

        for (GLsizei i = 0; i != sizeof(InstanceParameters) / sizeof(glm::vec4); ++i) {
            bindAttribute(
                array_attribute + i, 4, GL_FLOAT, sizeof(InstanceParameters),
                sizeof(glm::vec4) * i);// NOLINT
            glVertexAttribDivisor(array_attribute + i, divisor);
        }

        unbind();
    }
}// namespace mv::gl
