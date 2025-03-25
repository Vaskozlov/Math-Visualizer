#include <GL/glew.h>
#include <mutex>
#include <mv/gl/gl_init.hpp>

namespace mv::gl
{
    auto doInit() -> void
    {
        glewInit();
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
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
} // namespace mv::gl
