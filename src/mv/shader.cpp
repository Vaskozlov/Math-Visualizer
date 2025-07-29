#include <array>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <mv/shader.hpp>
#include <sstream>

namespace mv
{
    static auto shaderHeader() -> std::string
    {
#ifdef __EMSCRIPTEN__
        return "#version 300 es\n"
               "precision highp float;\n";
#else
        return "#version 330 core\n";
#endif
    }

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

        auto full_shader_code = shaderHeader();

        for (const auto &vertex_code_str : vertex_shaders) {
            full_shader_code += vertex_code_str;
            full_shader_code.push_back('\n');
        }

        shaders.emplace_back(compileShader(full_shader_code.c_str(), GL_VERTEX_SHADER));

        full_shader_code = shaderHeader();

        for (const auto &fragment_code_str : fragment_shaders) {
            full_shader_code += fragment_code_str;
            full_shader_code.push_back('\n');
        }

        shaders.emplace_back(compileShader(full_shader_code.c_str(), GL_FRAGMENT_SHADER));

        program = createProgram(shaders);

        for (const auto shader : shaders) {
            glDeleteShader(shader);
        }
    }
} // namespace mv
