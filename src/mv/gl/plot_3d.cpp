#include <glm/glm.hpp>
#include <mv/gl/axes_3d.hpp>
#include <numbers>

namespace mv::gl::shape
{
    Axes3D::Axes3D(const std::int32_t axis_size)
    {
        Prism axes{0.008F, static_cast<float>(axis_size) * 2.0F, 4};

        const auto original_points = axes.vertices;

        for (std::int32_t i = -axis_size; i != axis_size + 1; ++i) {
            auto model1 = glm::rotate(
                glm::translate(glm::mat4(1.0F), {static_cast<float>(i), 0.0F, 0.0F}),
                std::numbers::pi_v<float> / 2.0F,
                {1.0F, 0.0F, 0.0F});

            auto model2 = glm::rotate(
                glm::translate(glm::mat4(1.0F), {0.0F, static_cast<float>(i), 0.0F}),
                std::numbers::pi_v<float> / 2.0F, {0.0F, 1.0F, 0.0F});

            for (auto point : original_points) {
                axes.vertices.emplace_back(model1 * glm::vec4(point, 1.0F));
            }

            for (auto point : original_points) {
                axes.vertices.emplace_back(model2 * glm::vec4(point, 1.0F));
            }
        }

        vertices = std::move(axes.vertices);
    }
}// namespace mv::gl::shape
