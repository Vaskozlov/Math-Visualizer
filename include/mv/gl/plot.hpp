#ifndef MV_PLOT_HPP
#define MV_PLOT_HPP

#include <mv/gl/shape/prism.hpp>
#include <mv/gl/instancing.hpp>

namespace mv::gl::shape
{
    class Plot : public Shape
    {
    public:
        Prism mainAxis;
        Prism axis;

        explicit Plot(float plot_size);

        auto doDraw() const -> void final
        {
            // axis.doDraw();
        }
    };
}// namespace mv::gl::shape

#endif /* MV_PLOT_HPP */
