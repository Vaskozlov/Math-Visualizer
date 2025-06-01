#include <battery/embed.hpp>
#include <mv/gl/shaders/color_shader.hpp>

namespace mv::gl
{
    auto getColorShader() -> Shader *
    {
        static Shader colorShader{
            {b::embed<"resources/shaders/colored_shader.vert">().str()},
            {b::embed<"resources/shaders/fragment.frag">().str()},
        };

        return &colorShader;
    }

    auto getTexture3DLinearShader() -> Shader *
    {
        static Shader shader{
            {b::embed<"resources/shaders/tex3d/linear_with_fixed_level.vert">().str()},
            {
             b::embed<"resources/shaders/tex3d/linear_with_fixed_level.frag">().str(),
             b::embed<"resources/shaders/waterfall/linear/linear_rgb.glsl">().str(),
             },
        };

        return std::addressof(shader);
    }
} // namespace mv::gl
