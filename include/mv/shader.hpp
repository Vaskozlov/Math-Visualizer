#ifndef MV_SHADER_HPP
#define MV_SHADER_HPP

#include <GL/glew.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace mv
{
    class Shader
    {
    private:
        GLuint program;

    public:
        Shader(
            const std::vector<std::string> &vertex_shaders,
            const std::vector<std::string> &fragment_shaders);

        ~Shader()
        {
            glDeleteShader(program);
        }

        auto use() const -> void
        {
            glUseProgram(program);
        }

        auto setBool(const std::string &name, const bool value) const -> void
        {
            glUniform1i(glGetUniformLocation(program, name.c_str()), static_cast<int>(value));
        }

        auto setInt(const std::string &name, const int value) const -> void
        {
            glUniform1i(glGetUniformLocation(program, name.c_str()), value);
        }

        auto setFloat(const std::string &name, const float value) const -> void
        {
            glUniform1f(glGetUniformLocation(program, name.c_str()), value);
        }

        auto setMat4(const std::string &name, const glm::mat4 &value) const -> void
        {
            glUniformMatrix4fv(
                glGetUniformLocation(program, name.data()), 1, GL_FALSE, glm::value_ptr(value));
        }

        auto setVec2(const std::string &name, const glm::vec2 &value) const -> void
        {
            glUniform2f(glGetUniformLocation(program, name.c_str()), value.x, value.y);
        }

        auto setVec4(const std::string &name, const glm::vec4 &value) const -> void
        {
            glUniform4f(
                glGetUniformLocation(program, name.c_str()), value.x, value.y, value.z, value.w);
        }
    };
}// namespace mv

#endif /* MV_SHADER_HPP */
