#include "mv/shader.hpp"
#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace mv
{
    static auto compileShader(const char *shader_code, const GLenum shader_type) -> GLuint
    {
        int success = 0;
        std::array<char, 1024> info_log = {0};

        const GLuint shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shader_code, NULL);
        glCompileShader(shader);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == 0) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log.data());
            fmt::println("Failed to compile shader: {}", info_log.data());
            throw std::runtime_error("Failed to compile shader");
        }

        return shader;
    }

    template<std::size_t N>
    static auto createProgram(const std::array<GLuint, N> &shaders) -> GLuint
    {
        int success = 0;
        std::array<char, 1024> info_log = {0};
        GLuint program = glCreateProgram();

        for (auto shader : shaders) {
            glAttachShader(program, shader);
        }

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success == 0) {
            fmt::print("Failed to link program: {}", info_log.data());
            throw std::runtime_error("Failed to link program");
        }

        return program;
    }

    Shader::Shader(
        const std::string &vertex_code_str, const std::string &fragment_code_str)
    {
        const char *vertex_code = vertex_code_str.c_str();
        const char *fragment_code = fragment_code_str.c_str();

        auto vertex = compileShader(vertex_code, GL_VERTEX_SHADER);
        auto fragment = compileShader(fragment_code, GL_FRAGMENT_SHADER);

        program = createProgram<2>({vertex, fragment});

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
}// namespace mv
