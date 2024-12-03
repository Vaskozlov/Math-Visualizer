#ifndef MV_POLYGONS_SHAPE_HPP
#define MV_POLYGONS_SHAPE_HPP

#include <mv/gl/shape/shape.hpp>

namespace mv::gl::shape::detail
{
    class PolygonsShape : public Shape
    {
    public:
        auto doDraw() const -> void final
        {
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        }
    };
}// namespace mv::gl::shape::detail

#endif /* MV_POLYGONS_SHAPE_HPP */
