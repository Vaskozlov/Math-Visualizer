#ifndef MV_GL_SHADERS_COLOR_SHADER_HPP
#define MV_GL_SHADERS_COLOR_SHADER_HPP

#include <mv/shader.hpp>

namespace mv::gl
{
    auto getColorShader() -> Shader *;

    auto getTexture3DLinearShader() -> Shader *;
} // namespace mv::gl

#endif /* MV_GL_SHADERS_COLOR_SHADER_HPP */
