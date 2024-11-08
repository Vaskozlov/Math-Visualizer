#include <GL/glew.h>
#include <mv/gl/init.hpp>

namespace mv::gl
{
    auto doInit() -> void
    {
        glewInit();
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    auto init() -> void
    {
        static bool initialized = false;
        static std::mutex mutex;

        if (!initialized) {
            const auto lock = std::unique_lock{mutex};

            if (initialized) {
                return;
            }

            initialized = true;
            doInit();
        }
    }
}// namespace mv::gl
