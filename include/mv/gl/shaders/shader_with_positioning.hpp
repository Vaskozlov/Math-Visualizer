#ifndef MV_GL_SHADERS_SHADER_WITH_POSITIONING_HPP
#define MV_GL_SHADERS_SHADER_WITH_POSITIONING_HPP

#include <mv/shader.hpp>

namespace mv::gl
{
    auto getShaderWithPositioning() -> Shader *;

    auto getHsvShaderWithModel() -> Shader *;

    auto getLinearShaderWithModel() -> Shader *;
} // namespace mv::gl

#endif /* MV_GL_SHADERS_SHADER_WITH_POSITIONING_HPP */
