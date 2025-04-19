#include <battery/embed.hpp>
#include <mv/waterfall_application.hpp>

namespace mv
{
    auto Waterfall::getWaterfallShaderHsvF32() -> Shader *
    {
        static Shader waterfallShaderHsvF32{
            {
             b::embed<"resources/shaders/waterfall/vertex_with_limits_f32.vert">().str(),
             },
            {
             b::embed<"resources/shaders/waterfall/hsv/f32_rgba.frag">().str(),
             b::embed<"resources/shaders/waterfall/hsv/hsv2rgb.glsl">().str(),
             },
        };

        return &waterfallShaderHsvF32;
    }

    auto Waterfall::getWaterfallShaderLinearF32() -> Shader *
    {
        static Shader waterfallShaderLinearF32{
            {
             b::embed<"resources/shaders/waterfall/vertex_with_limits_f32.vert">().str(),
             },
            {
             b::embed<"resources/shaders/waterfall/linear/f32_rgba.frag">().str(),
             b::embed<"resources/shaders/waterfall/linear/linear_rgb.glsl">().str(),
             },
        };

        return &waterfallShaderLinearF32;
    }

    auto Waterfall::init() -> void
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        GLint max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

        maxTextureSize = static_cast<std::size_t>(max_texture_size);
        camera.movementSpeed /= 10.0F;

        ImGui::StyleColorsDark();
        font = loadFont(30.0F);

        powerMapSize.loadData();
        powerMapSize.vbo.bind();
        powerMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        powerMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        azimuthMapSize.loadData();
        azimuthMapSize.vbo.bind();
        azimuthMapSize.vao.bind(0, 3, GL_FLOAT, 3 * sizeof(float), 0);
        azimuthMapSize.vao.bind(1, 2, GL_FLOAT, 3 * sizeof(float), 0);

        rectangle.loadData();

        rectangle.vbo.bind();
        rectangle.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        rectangleInstances.vbo.bind();
        rectangle.vao.bindInstanceParameters(1, 1);

        camera.setPosition({0.0F, 0.0F, 1.0F});

        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.reload();
        }

        for (auto &waterfall : powerWaterfalls) {
            waterfall.reload();
        }

        azimuthWaterfallMask.reload();
        powerWaterfallMask.reload();

        updateAzimuthUniform();
        updatePowerUniform();
    }
} // namespace mv
