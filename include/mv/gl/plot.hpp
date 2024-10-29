#ifndef MV_PLOT_HPP
#define MV_PLOT_HPP

#include <mv/gl/shape/detail/lines_shape.hpp>
#include <mv/gl/vertices_binder.hpp>

namespace mv::gl::shape
{
    class Plot : public Shape
    {
    private:
        float minX;
        float minY;
        float minZ;
        float maxX;
        float maxY;
        float maxZ;

    public:
        detail::LinesShape mainAxis;
        detail::LinesShape gird;

        explicit Plot(const float plot_size)
          : Plot(-plot_size, -plot_size, -plot_size, plot_size, plot_size, plot_size)
        {}

        Plot(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);

        auto doDraw() const -> void final
        {
            mainAxis.doDraw();
            gird.doDraw();
        }
    };
}// namespace mv::gl::shape

#endif /* MV_PLOT_HPP */
