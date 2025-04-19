#include <battery/embed.hpp>
#include <mv/gl/shaders/shader_with_positioning.hpp>

namespace mv::gl
{
    auto getShaderWithPositioning() -> Shader *
    {
        static Shader shaderWithPositioning{
            {b::embed<"resources/shaders/static_instance.vert">().str()},
            {b::embed<"resources/shaders/fragment.frag">().str()},
        };

        return &shaderWithPositioning;
    }

    auto getHsvShaderWithModel() -> Shader *
    {
        static Shader shader{
            {
             b::embed<"resources/shaders/waterfall/hsv_static.vert">().str(),
             },
            {
             b::embed<"resources/shaders/waterfall/hsv/hsv_static.frag">().str(),
             b::embed<"resources/shaders/waterfall/hsv/hsv2rgb.glsl">().str(),
             },
        };

        return &shader;
    }
} // namespace mv::gl
