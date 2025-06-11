#include <mv/application.hpp>

namespace mv
{
    auto Application::getShaderWithPositioning() const -> Shader
    {
        return Shader{
            {getResourceAsString("shaders/static_instance.vert")},
            {getResourceAsString("shaders/fragment.frag")},
        };
    }

    auto Application::getHsvShaderWithModel() const -> Shader
    {
        return Shader{
            {
             getResourceAsString("shaders/waterfall/static.vert"),
             },
            {
             getResourceAsString("shaders/waterfall/hsv/hsv_static.frag"),
             getResourceAsString("shaders/waterfall/hsv/hsv2rgb.glsl"),
             },
        };
    }

    auto Application::getLinearShaderWithModel() const -> Shader
    {
        return Shader{
            {
             getResourceAsString("shaders/waterfall/static.vert"),
             },
            {
             getResourceAsString("shaders/waterfall/linear/linear_static.frag"),
             getResourceAsString("shaders/waterfall/linear/linear_rgb.glsl"),
             },
        };
    }
} // namespace mv
