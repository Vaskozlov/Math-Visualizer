#include <mv/waterfall_application.hpp>

namespace mv
{
    auto Waterfall::processInput() -> void
    {
        constexpr static auto key_press_delay = 0.2;

        if (!disableInput) {
            Application2D::processInput();
        }

        const auto left_shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        const auto key_g_pressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

        if (left_shift_pressed && key_g_pressed) {
            const auto mode = glfwGetInputMode(window, GLFW_CURSOR);
            const double new_press_time = glfwGetTime();

            if (new_press_time - pressTime < key_press_delay) {
                return;
            }

            pressTime = new_press_time;
            firstMouse = true;
            isMouseShowed = mode == GLFW_CURSOR_DISABLED;

            glfwSetInputMode(
                window, GLFW_CURSOR, isMouseShowed ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
    }

    auto Waterfall::onMouseRelativeMovement(const double delta_x, const double delta_y) -> void
    {
        const auto scale = camera.getZoom() / 200.0F;

        camera.relativeMove(
            camera.getUp() * static_cast<float>(delta_y) * scale / imageHeightScale, deltaTime);

        camera.relativeMove(
            camera.getRight() * static_cast<float>(delta_x) * scale / imageWidthScale, deltaTime);
    }

    auto Waterfall::changeWidthScale(const float scale_difference) -> void
    {
        imageWidthScale += scale_difference;
        imageWidthScale = std::clamp(imageWidthScale, minWidthScale, maxWidthScale);
    }

    auto Waterfall::changeHeightScale(const float scale_difference) -> void
    {
        imageHeightScale += scale_difference;
        imageHeightScale = std::clamp(imageHeightScale, minHeightScale, maxHeightScale);
        drawDetections();
    }

    auto Waterfall::onScroll(const double x_offset, const double y_offset) -> void
    {
        const int v_key_state = glfwGetKey(window, GLFW_KEY_V);
        const int h_key_state = glfwGetKey(window, GLFW_KEY_H);
        const int shift_key_state = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
        const auto scale = static_cast<double>(camera.getZoom()) / 1000.0;

        if ((shift_key_state == GLFW_PRESS && h_key_state == GLFW_PRESS)
            || (v_key_state == GLFW_FALSE && h_key_state == GLFW_FALSE)) {
            changeWidthScale(static_cast<float>(x_offset + y_offset) * scale);
        }

        if ((shift_key_state == GLFW_PRESS && v_key_state == GLFW_PRESS)
            || (v_key_state == GLFW_FALSE && h_key_state == GLFW_FALSE)) {
            changeHeightScale(
                static_cast<float>(x_offset + y_offset) * scale * maxHeightScale / maxWidthScale);
        }
    }
} // namespace mv
