#include <mv/application_3d.hpp>

namespace mv
{
    auto Application3D::processInput() -> void
    {
        using enum CameraMovement;
        Application::processInput();

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.moveForward(deltaTime);
            glfwPostEmptyEvent();
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.moveBack(deltaTime);
            glfwPostEmptyEvent();
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.moveLeft(deltaTime);
            glfwPostEmptyEvent();
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.moveRight(deltaTime);
            glfwPostEmptyEvent();
        }
    }
} // namespace mv
