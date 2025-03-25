#ifndef MV_SHAPE_RECTANGLE_HPP
#define MV_SHAPE_RECTANGLE_HPP

#include <mv/gl/shape/detail/polygon.hpp>

namespace mv::gl::shape
{
    class Rectangle : public detail::Polygon
    {
    public:
        Rectangle(const float x, const float y, const float width, const float height)
          : Polygon{
                {x + width, y + height, 0.0F},
                {x + width, y,          0.0F},
                {x,         y,          0.0F},
                {x + width, y + height, 0.0F},
                {x,         y,          0.0F},
                {x,         y + height, 0.0F},
        }
        {}
    };
} // namespace mv::gl::shape

#endif /* MV_SHAPE_RECTANGLE_HPP */
