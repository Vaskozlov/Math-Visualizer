#ifndef MV_PLOT_2D_HPP
#define MV_PLOT_2D_HPP

#include <glm/glm.hpp>
#include <mv/gl/shape/detail/polygon.hpp>
#include <span>

namespace mv::gl::shape
{
    class Plot2D : public detail::Polygon
    {
    public:
        Plot2D() = default;

        explicit Plot2D(
            std::span<const float> x, std::span<const float> y, float thickness = 0.03F);

        auto fill(std::span<const float> x, std::span<const float> y, float thickness = 0.03F)
            -> void;
    };
}// namespace mv::gl::shape

#endif /* MV_PLOT_2D_HPP */
