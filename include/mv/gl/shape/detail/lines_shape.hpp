#ifndef MV_LINES_SHAPE_HPP
#define MV_LINES_SHAPE_HPP

#include <mv/gl/shape/shape.hpp>

namespace mv::gl::shape::detail
{
    class LinesShape : public Shape
    {
    public:
        auto doDraw() const -> void final
        {
            glDrawArrays(GL_LINE, 0, vertices.size());
        }
    };
}// namespace mv::gl::shape::detail

#endif /* MV_LINES_SHAPE_HPP */
