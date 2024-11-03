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
    };

    class Camera
    {
    private:
        constexpr static float YAW = -90.0F;
        constexpr static float PITCH = 0.0F;
        constexpr static float SPEED = 2.5F;
        constexpr static float SENSITIVITY = 0.1F;
        constexpr static float ZOOM = 45.0F;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 worldUp{};

        float yaw;
        float pitch;
        float movementSpeed;
        float mouseSensitivity;
        float zoom;

        float rotationAngle = 0.0F;
        float rotationAngleY = 0.0F;

    public:
        explicit Camera(
            const glm::vec3 position = glm::vec3(5.0F, 5.0F, 5.0F),
            const glm::vec3 camera_up = glm::vec3(0.0F, 1.0F, 0.0F), const float camera_yaw = YAW,
            const float camera_pitch = PITCH)
          : position{position}
          , front{glm::vec3(0.0F, 0.0F, -1.0F)}
          , worldUp{camera_up}
          , yaw{camera_yaw}
          , pitch{camera_pitch}
          , movementSpeed{SPEED}
          , mouseSensitivity{SENSITIVITY}
          , zoom{ZOOM}
        {
            updateCameraVectors();
        }


        [[nodiscard]] auto getViewMatrix() const -> glm::mat4
        {
            return glm::rotate(
                glm::lookAt(position, position + front, up), rotationAngle,
                glm::vec3(0.0F, 1.0F, 0.0F));
        }

        [[nodiscard]] auto getZoom() const noexcept -> float
        {
            return zoom;
        }

        auto rotate(const double x_pos_in, const double y_pos_in) -> void
        {
            rotationAngle += glm::radians(x_pos_in);
            rotationAngleY -= glm::radians(y_pos_in);
        }

        auto processMouseMovement(
            float xOffset, float yOffset, const GLboolean constrainPitch = true)
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

        auto processKeyboard(const CameraMovement direction, const float deltaTime) -> void
        {
            const float velocity = movementSpeed * deltaTime;

            switch (direction) {
            case CameraMovement::FORWARD:
                position += front * velocity;
                break;

            case CameraMovement::BACKWARD:
                position -= front * velocity;
                break;

            case CameraMovement::LEFT:
                position -= right * velocity;
                break;

            case CameraMovement::RIGHT:
                position += right * velocity;
                break;

            default:
                isl::unreachable();
            }
        }

        auto processMouseScroll(const float yOffset) -> void
        {
            zoom -= yOffset;
            zoom = std::max(zoom, 1.0F);
            zoom = std::min(zoom, 90.0F);
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
