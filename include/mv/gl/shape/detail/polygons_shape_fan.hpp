#ifndef MV_POLYGONS_SHAPE_FAN_HPP
#define MV_POLYGONS_SHAPE_FAN_HPP

#include <mv/gl/shape/shape.hpp>

namespace mv::gl::shape::detail
{
    class PolygonsShapeFan : public Shape
    {
    public:
        auto doDraw() const -> void final
        {
            glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
        }
    };
}// namespace mv::gl::shape::detail

#endif /* MV_POLYGONS_SHAPE_FAN_HPP */
