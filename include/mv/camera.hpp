#ifndef MV_CAMERA_HPP
#define MV_CAMERA_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <isl/isl.hpp>

namespace mv
{
    enum class CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN,
    };

    class Camera
    {
    private:
        constexpr static float YAW = -90.0F;
        constexpr static float PITCH = 0.0F;
        constexpr static float SPEED = 2.5F;
        constexpr static float SENSITIVITY = 0.1F;
        constexpr static float ZOOM = 45.0F;
        constexpr static float MAX_ZOOM = 0.001F;
        constexpr static float MIN_ZOOM = 120.0F;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};

        float yaw;
        float pitch;
        float mouseSensitivity;
        float zoom;

        float rotationAngle = 0.0F;
        float rotationAngleY = 0.0F;

    public:
        float movementSpeed;

        explicit Camera(
            const glm::vec3 position = glm::vec3(0.0F, 0.0F, 2.0F),
            const glm::vec3 camera_up = glm::vec3(0.0F, 1.0F, 0.0F), const float camera_yaw = YAW,
            const float camera_pitch = PITCH)
          : position{position}
          , front{glm::vec3(0.0F, 0.0F, -1.0F)}
          , worldUp{camera_up}
          , yaw{camera_yaw}
          , pitch{camera_pitch}
          , mouseSensitivity{SENSITIVITY}
          , zoom{ZOOM}
          , movementSpeed{SPEED}
        {
            updateCameraVectors();
        }

        [[nodiscard]] auto getViewMatrix() const -> glm::mat4
        {
            return glm::rotate(
                glm::lookAt(position, position + front, up), rotationAngle,
                glm::vec3(0.0F, 1.0F, 0.0F));
        }

        [[nodiscard]] auto getUp() const noexcept -> glm::vec3
        {
            return up;
        }

        [[nodiscard]] auto getRight() const noexcept -> glm::vec3
        {
            return right;
        }

        [[nodiscard]] auto getFront() const noexcept -> glm::vec3
        {
            return front;
        }

        [[nodiscard]] auto getZoom() const noexcept -> float
        {
            return zoom;
        }

        [[nodiscard]] auto getPosition() const noexcept -> glm::vec3
        {
            return position;
        }

        auto setPosition(const glm::vec3 &new_position) -> void
        {
            position = new_position;
        }

        auto rotate(const double x_pos_in, const double y_pos_in) -> void
        {
            rotationAngle += glm::radians(static_cast<float>(x_pos_in));
            rotationAngleY -= glm::radians(static_cast<float>(y_pos_in));
        }

        auto processMouseMovement(
            float xOffset, float yOffset, const GLboolean constrainPitch = GLFW_TRUE) -> void
        {
            xOffset *= mouseSensitivity;
            yOffset *= mouseSensitivity;

            yaw += xOffset;
            pitch += yOffset;

            if (constrainPitch != 0U) {
                pitch = std::min(pitch, 89.0F);
                pitch = std::max(pitch, -89.0F);
            }

            updateCameraVectors();
        }

        auto moveLeft(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position -= right * velocity;
        }

        auto moveRight(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position += right * velocity;
        }

        auto moveUp(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position += up * velocity;
        }

        auto moveDown(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position -= up * velocity;
        }

        auto moveForward(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position += front * velocity;
        }

        auto moveBack(const float delta_time, const float multiplier = 1.0F) -> void
        {
            const float velocity = movementSpeed * delta_time * multiplier;
            position -= front * velocity;
        }

        auto relativeMove(const glm::vec3 &direction, const float delta_time) -> void
        {
            position += direction * movementSpeed * delta_time;
        }

        auto relativeMove(const glm::vec3 &direction) -> void
        {
            position += direction;
        }

        auto processMouseScroll(const float yOffset) -> void
        {
            zoom -= yOffset;
            zoom = std::max(zoom, MAX_ZOOM);
            zoom = std::min(zoom, MIN_ZOOM);
        }

    private:
        auto updateCameraVectors() -> void
        {
            front = glm::normalize(
                glm::vec3{
                    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                    sin(glm::radians(pitch)),
                    sin(glm::radians(yaw)) * cos(glm::radians(pitch)),
                });

            right = glm::normalize(glm::cross(front, worldUp));
            up = glm::normalize(glm::cross(right, front));
        }
    };
}// namespace mv

#endif /* MV_CAMERA_HPP */
