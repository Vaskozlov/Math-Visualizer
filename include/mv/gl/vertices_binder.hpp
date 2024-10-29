#ifndef MV_VBO_HPP
#define MV_VBO_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <isl/isl.hpp>
#include <mv/shader.hpp>

namespace mv::gl
{
    template<typename T>
    class VerticesContainer
    {
    public:
        std::vector<T> vertices;

    private:
        GLuint VBO_ID = 0;
        GLuint VAO_ID = 0;
        bool initialized = false;

    public:
        VerticesContainer()
        {
            glGenVertexArrays(1, &VAO_ID);
            glGenBuffers(1, &VBO_ID);
        }

        VerticesContainer(const std::initializer_list<T> &initial_points)
          : vertices(initial_points)
        {
            glGenVertexArrays(1, &VAO_ID);
            glGenBuffers(1, &VBO_ID);
        }

        virtual ~VerticesContainer()
        {
            glDeleteBuffers(1, &VBO_ID);
            glDeleteVertexArrays(1, &VAO_ID);
        }

        auto bind(
            const GLsizei array_attribute, const GLuint index, const GLint size, const GLenum type,
            const GLsizei stride, const GLsizei offset) const -> void
        {
            bindVao();
            bindVbo();

            glVertexAttribPointer(
                index, size, type, GL_FALSE, stride,
                static_cast<const void *>(static_cast<const char *>(0x0) + offset));// NOLINT

            glEnableVertexAttribArray(array_attribute);
            unbindVao();
        }

        static auto unbindVao() -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        auto loadData(const GLenum mode = GL_STATIC_DRAW) -> void
        {
            glBindVertexArray(VAO_ID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);

            if (!initialized) {
                initialized = true;
                glBufferData(
                    GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), mode);
            } else {
                glBufferSubData(
                    GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        auto bindVbo() const -> void
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
        }

        auto bindVao() const -> void
        {
            glBindVertexArray(VAO_ID);
        }
    };
}// namespace mv::gl

#endif /* MV_VBO_HPP */
