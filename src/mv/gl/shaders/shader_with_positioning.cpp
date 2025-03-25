#include <battery/embed.hpp>
#include <mv/gl/shaders/shader_with_positioning.hpp>

namespace mv::gl
{
    auto getShaderWithPositioning() -> Shader &
    {
        static Shader shaderWithPositioning{
            {b::embed<"resources/shaders/static_instance.vert">().str()},
            {b::embed<"resources/shaders/fragment.frag">().str()},
        };

        return shaderWithPositioning;
    }
} // namespace mv::gl
