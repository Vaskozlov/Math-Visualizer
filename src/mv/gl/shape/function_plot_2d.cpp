#include <mv/gl/shape/plot_2d.hpp>

namespace mv::gl::shape
{
    [[nodiscard]] static auto
        createThickLineDots(const glm::vec2 first, const glm::vec2 second, const float thickness)
            -> std::array<glm::vec2, 2>
    {
        const auto t = thickness * 0.5F;
        const auto path = second - first;
        const auto normal = glm::normalize(glm::vec2{-path.y, path.x}) * t;

        return {first + normal, first - normal};
    }

    Plot2D::Plot2D(
        const std::span<const float> x, const std::span<const float> y, const float thickness)
    {
        fill(x, y, thickness);
    }

    auto Plot2D::fill(
        const std::span<const float> x, const std::span<const float> y, const float thickness)
        -> void
    {
        assert(x.size() == y.size());

        vertices.clear();

        if (x.size() <= 1) {
            return;
        }

        vertices.reserve(x.size() * 6);

        const auto initial_points = createThickLineDots({x[0], y[0]}, {x[1], y[1]}, thickness);

        auto p1 = glm::vec3{initial_points[0].x, 0.0F, initial_points[0].y};
        auto p2 = glm::vec3{initial_points[1].x, 0.0F, initial_points[1].y};

        for (size_t i = 1; i < x.size(); ++i) {
            auto points_fragment =
                createThickLineDots({x[i - 1], y[i - 1]}, {x[i], y[i]}, thickness);

            auto p3 = glm::vec3{points_fragment[0].x, 0.0F, points_fragment[0].y};
            auto p4 = glm::vec3{points_fragment[1].x, 0.0F, points_fragment[1].y};

            vertices.emplace_back(p1);
            vertices.emplace_back(p2);
            vertices.emplace_back(p4);

            vertices.emplace_back(p1);
            vertices.emplace_back(p3);
            vertices.emplace_back(p4);

            p1 = p3;
            p2 = p4;
        }
    }
}// namespace mv::gl::shape
