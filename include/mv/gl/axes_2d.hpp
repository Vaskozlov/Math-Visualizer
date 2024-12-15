#ifndef MV_AXES_2D_HPP
#define MV_AXES_2D_HPP

#include <mv/gl/shape/prism.hpp>

namespace mv::gl::shape
{
    class Axes2D : public detail::Polygon
    {
    public:
        explicit Axes2D(std::int32_t axis_size);
    };
}// namespace mv::gl::shape

#endif /* MV_AXES_2D_HPP */
