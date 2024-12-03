#include <glm/glm.hpp>
#include <mv/gl/plot.hpp>

namespace mv::gl::shape {
    Plot::Plot(const std::int32_t plot_size)
        : Prism{0.008F, static_cast<float>(plot_size) * 2.0F, 4} {
        const auto original_points = vertices;

        for (std::int32_t i = -plot_size; i != plot_size + 1; ++i) {
            auto model1 = glm::rotate(
                glm::translate(glm::mat4(1.0F), {static_cast<float>(i), 0.0F, 0.0F}),
                static_cast<float>(M_PI_2),
                {1.0F, 0.0F, 0.0F});

            auto model2 = glm::rotate(
                glm::translate(glm::mat4(1.0F), {0.0F, static_cast<float>(i), 0.0F}),
                static_cast<float>(M_PI_2),
                {0.0F, 1.0F, 0.0F});

            for (auto point: original_points) {
                vertices.emplace_back(model1 * glm::vec4(point, 1.0F));
            }

            for (auto point: original_points) {
                vertices.emplace_back(model2 * glm::vec4(point, 1.0F));
            }
        }
    }
} // namespace mv::gl::shape
