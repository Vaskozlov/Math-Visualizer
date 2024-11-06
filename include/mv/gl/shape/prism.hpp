#ifndef MV_PRISM_HPP
#define MV_PRISM_HPP

#include <mv/gl/shape/detail/polygons_shape.hpp>

namespace mv::gl::shape
{
    class Prism : public detail::PolygonsShape
    {
    public:
        explicit Prism(float radius, float height, uint32_t vertices_count);
    };
}// namespace mv::gl::shape

#endif /* MV_PRISM_HPP */
