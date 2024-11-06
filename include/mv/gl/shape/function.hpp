#ifndef MV_FUNCTION_HPP
#define MV_FUNCTION_HPP

#include <mv/gl/shape/detail/polygons_shape.hpp>

namespace mv::gl::shape
{
    class Function final : public detail::PolygonsShape
    {
    private:
        constexpr static auto step = 0.05F;

    public:
        Function(float (*function)(glm::vec2), const glm::vec4 space)
          : Function(function, -space.x, -space.y, space.z, space.w)
        {}

        Function(
            float (*function)(glm::vec2), const float min_x, const float min_y, const float max_x,
            const float max_y)
        {
            const auto from_x = std::floor(min_x);
            const auto from_y = std::floor(min_y);
            const auto to_x = std::ceil(max_x);
            const auto to_y = std::ceil(max_y);

            const auto x_steps = static_cast<std::size_t>((to_x - from_x) / step);
            const auto y_steps = static_cast<std::size_t>((to_y - from_y) / step);

            vertices.reserve(x_steps * y_steps * 6U);

            for (std::size_t x_index = 0; x_index != x_steps; ++x_index) {
                const auto x = from_x + (step * x_index);
                const auto next_x = x + step;

                assert(
                    x != next_x &&
                    "Floating point precision is not enough to compute this function");

                auto y = from_y;
                auto next_y = y + step;

                for (std::size_t y_index = 0; y_index != y_steps; ++y_index) {
                    auto p0 = glm::vec3(x, y, function({x, y}));
                    auto p1 = glm::vec3(x, next_y, function({x, next_y}));
                    auto p2 = glm::vec3(next_x, y, function({next_x, y}));
                    auto p3 = glm::vec3(next_x, next_y, function({next_x, next_y}));

                    vertices.push_back(p0);
                    vertices.push_back(p1);
                    vertices.push_back(p2);

                    vertices.push_back(p3);
                    vertices.push_back(p2);
                    vertices.push_back(p1);

                    y = std::exchange(next_y, next_y + step);

                    assert(
                        y != next_y &&
                        "Floating point precision is not enough to compute this function");
                }
            }
        }
    };
}// namespace mv::gl::shape

#endif /* MV_FUNCTION_HPP */
