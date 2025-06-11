#include <mv/application.hpp>

namespace mv
{
    auto Application::getColorShader() const -> Shader
    {
        return Shader{
            {getResourceAsString("shaders/colored_shader.vert")},
            {getResourceAsString("shaders/fragment.frag")},
        };
    }

    auto Application::getTexture3DLinearShader() const -> Shader
    {
        return Shader{
            {
             getResourceAsString("shaders/tex3d/linear_with_fixed_level.vert"),
             },
            {
             getResourceAsString("shaders/tex3d/linear_with_fixed_level.frag"),
             getResourceAsString("shaders/waterfall/linear/linear_rgb.glsl"),
             },
        };
    }
} // namespace mv
