#ifndef MV_SHADER_HPP
#define MV_SHADER_HPP

#include <filesystem>
#include <GL/glew.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mv
{
    class Shader
    {
    private:
        GLuint program;

    public:
        Shader(const std::string &vertex_code_str, const std::string &fragment_code_str);

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
            glUniform1i(glGetUniformLocation(program, name.data()), value);
        }

        auto setFloat(const std::string &name, const float value) const -> void
        {
            glUniform1f(glGetUniformLocation(program, name.data()), value);
        }

        auto setMat4(const std::string &name, const glm::mat4 &value) const -> void
        {
            glUniformMatrix4fv(
                glGetUniformLocation(program, name.data()), 1, GL_FALSE, glm::value_ptr(value));
        }

        auto setVec4(const std::string &name, const glm::vec4 &value) const -> void
        {
            glUniform4f(
                glGetUniformLocation(program, name.data()), value.x, value.y, value.z, value.w);
        }
    };
}// namespace mv

#endif /* MV_SHADER_HPP */
