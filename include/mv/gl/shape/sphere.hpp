#ifndef MV_SPHERE_HPP
#define MV_SPHERE_HPP

#include <isl/isl.hpp>
#include <mv/gl/shape/detail/polygon.hpp>

namespace mv::gl::shape
{
    class Sphere final : public detail::Polygon
    {
    public:
        constexpr static isl::u32 defaultSlicesCount = 26U;
        constexpr static isl::u32 defaultStacksCount = 26U;

        explicit Sphere(
            float radius, isl::u32 slices = defaultSlicesCount,
            isl::u32 stacks = defaultStacksCount);

        auto doDraw() const -> void override
        {
            glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
        }
    };
}// namespace mv::gl::shape

#endif /* MV_SPHERE_HPP */
