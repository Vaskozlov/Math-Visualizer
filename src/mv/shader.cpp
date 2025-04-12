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

    static auto createProgram(const std::vector<GLuint> &shaders) -> GLuint
    {
        int success = 0;
        std::array<char, 1024> info_log = {0};
        const GLuint program = glCreateProgram();

        for (const auto shader : shaders) {
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
        const std::vector<std::string> &vertex_shaders,
        const std::vector<std::string> &fragment_shaders)
    {
        std::vector<GLuint> shaders;

        for (const auto &vertex_code_str : vertex_shaders) {
            const char *vertex_shader_code = vertex_code_str.c_str();
            shaders.emplace_back(compileShader(vertex_shader_code, GL_VERTEX_SHADER));
        }

        for (const auto &fragment_code_str : fragment_shaders) {
            const char *fragment_shader_code = fragment_code_str.c_str();
            shaders.emplace_back(compileShader(fragment_shader_code, GL_FRAGMENT_SHADER));
        }

        program = createProgram(shaders);

        for (const auto shader : shaders) {
            glDeleteShader(shader);
        }
    }
} // namespace mv
