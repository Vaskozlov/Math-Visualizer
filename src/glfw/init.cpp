#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <mutex>
#include <mv/glfw/init.hpp>

namespace mv::glfw
{
    static auto doInit(const int major_version, const int minor_version) -> void
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    auto init(const int major_version, const int minor_version) -> void
    {
        static bool initialized = false;
        static std::mutex mutex;

        if (!initialized) {
            const auto lock = std::scoped_lock{mutex};

            if (initialized) {
                return;
            }

            initialized = true;
            doInit(major_version, minor_version);
        }
    }
}// namespace mv::glfw
