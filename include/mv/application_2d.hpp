#ifndef MV_APPLICATION_2D_HPP
#define MV_APPLICATION_2D_HPP

#include <mv/application.hpp>

namespace mv
{
    class Application2D : public Application
    {
    public:
        using Application::Application;

        auto processInput() -> void override;
    };
}// namespace mv

#endif /* MV_APPLICATION_2D_HPP */
