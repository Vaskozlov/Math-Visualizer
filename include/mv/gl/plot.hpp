#ifndef MV_PLOT_HPP
#define MV_PLOT_HPP

#include <mv/gl/shape/prism.hpp>

namespace mv::gl::shape
{
    class Plot : public Prism
    {
    public:
        explicit Plot(float plot_size);
    };
}// namespace mv::gl::shape

#endif /* MV_PLOT_HPP */
