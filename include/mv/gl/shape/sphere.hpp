#ifndef MV_SPHERE_HPP
#define MV_SPHERE_HPP

#include <isl/isl.hpp>
#include <mv/gl/shape/detail/polygons_shape.hpp>

namespace mv::gl::shape
{
    class Sphere final : public detail::PolygonsShape
    {
    public:
        explicit Sphere(const float radius)
          : Sphere(radius, static_cast<isl::u32>(26), static_cast<isl::u32>(26))
        {}

        Sphere(float radius, isl::u32 slices, isl::u32 stacks);
    };
}// namespace mv::gl::shape

#endif /* MV_SPHERE_HPP */
