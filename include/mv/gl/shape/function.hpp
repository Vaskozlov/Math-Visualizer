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
        Function(float (*function)(glm::vec2), const float space_size)
          : Function(
                function, -space_size, -space_size, -space_size, space_size, space_size, space_size)
        {}

        Function(
            float (*function)(glm::vec2), const float min_x, const float min_y, const float min_z,
            const float max_x, const float max_y, const float max_z)
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
                    auto f0 = function({x, y});
                    auto f1 = function({x, next_y});
                    auto f2 = function({next_x, y});
                    auto f3 = function({next_x, next_y});

                    if (f0 > max_z && f1 > max_z && f2 > max_z && f3 > max_z) {
                        y = std::exchange(next_y, next_y + step);
                        continue;
                    }

                    if (f0 < min_z && f1 < min_z && f2 < min_z && f3 < min_z) {
                        y = std::exchange(next_y, next_y + step);
                        continue;
                    }

                    f0 = std::max(std::min(f0, max_z), min_z);
                    f1 = std::max(std::min(f1, max_z), min_z);
                    f2 = std::max(std::min(f2, max_z), min_z);
                    f3 = std::max(std::min(f3, max_z), min_z);

                    const auto p0 = glm::vec3(x, y, f0);
                    const auto p1 = glm::vec3(x, next_y, f1);
                    const auto p2 = glm::vec3(next_x, y, f2);
                    const auto p3 = glm::vec3(next_x, next_y, f3);

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
