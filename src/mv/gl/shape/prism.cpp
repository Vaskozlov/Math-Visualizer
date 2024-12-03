#include <mv/gl/shape/prism.hpp>

namespace mv::gl::shape {
    Prism::Prism(const float radius, const float height, const uint32_t vertices_count) {
        const float step = static_cast<float>(M_PI) * 2.0F / static_cast<float>(vertices_count);
        const float middle_y = height / 2.0F;

        for (uint32_t i = 0; i != vertices_count; ++i) {
            const auto angle = step * static_cast<float>(i);
            const auto p0 =
                    glm::vec3(radius * std::cos(angle), radius * std::sin(angle), -middle_y);

            const auto p1 = glm::vec3(
                radius * std::cos(angle + step), radius * std::sin(angle + step), -middle_y);

            const auto p2 = glm::vec3(
                radius * std::cos(angle + step), radius * std::sin(angle + step), middle_y);

            const auto p3 = glm::vec3(radius * std::cos(angle), radius * std::sin(angle), middle_y);

            vertices.emplace_back(p0);
            vertices.emplace_back(p1);
            vertices.emplace_back(p2);

            vertices.emplace_back(p3);
            vertices.emplace_back(p2);
            vertices.emplace_back(p0);
        }
    }
} // namespace mv::gl::shape
