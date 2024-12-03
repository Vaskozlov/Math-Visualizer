#include <mv/gl/shape/sphere.hpp>
#include <numbers>

namespace mv::gl::shape
{
    static auto computeSphereCoordinate(const float u, const float v, const float r) -> glm::vec3
    {
        return r * glm::vec3{cos(u) * sin(v), cos(v), sin(v) * sin(u)};
    }

    Sphere::Sphere(const float radius, const isl::u32 slices, const isl::u32 stacks)
    {
        constexpr float u_start = 0.0F;
        constexpr float v_start = 0.0F;

        constexpr float u_end = std::numbers::pi_v<float> * 2.0F;
        constexpr float v_end = std::numbers::pi_v<float>;

        const float u_step = (u_end - u_start) / static_cast<float>(slices);
        const float v_step = (v_end - v_start) / static_cast<float>(stacks);

        vertices.reserve(slices * stacks * 2U);

        for (isl::u32 i = 0; i != slices + 1; ++i) {
            for (isl::u32 j = 0; j != stacks + 1; ++j) {
                const float u = (static_cast<float>(i) * u_step) + u_start;
                const float v = (static_cast<float>(j) * v_step) + v_start;

                const float next_u = std::min(u + u_step, u_end);

                const auto p0 = computeSphereCoordinate(u, v, radius);
                const auto p1 = computeSphereCoordinate(next_u, v, radius);

                vertices.emplace_back(p0);
                vertices.emplace_back(p1);
            }
        }

        loadData();
    }
}// namespace mv::gl::shape