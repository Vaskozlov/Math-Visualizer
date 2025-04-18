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
} // namespace mv::gl
