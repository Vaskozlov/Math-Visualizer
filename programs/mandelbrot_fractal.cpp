#include <battery/embed.hpp>
#include <imgui.h>
#include <mv/application_2d.hpp>
#include <mv/gl/instancing.hpp>
#include <mv/gl/vertices_binder.hpp>
#include <mv/shader.hpp>

class MandelbrotFractalApplication final : public mv::Application2D
{
private:
    mv::Shader mandelbrotFractalShader = mv::Shader{
        b::embed<"resources/shaders/fractal.vert">().str(),
        b::embed<"resources/shaders/fractal.frag">().str(),
    };

    mv::gl::VerticesContainer<glm::vec3> vertices{
        {2.0F, 2.0F, 0.0F}, {2.0F, -2.0F, 0.0F},  {-2.0F, -2.0F, 0.0F},
        {2.0F, 2.0F, 0.0F}, {-2.0F, -2.0F, 0.0F}, {-2.0F, 2.0F, 0.0F},
    };

    double pressTime = 0.0;

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        vertices.loadData();

        mandelbrotFractalShader.use();
        vertices.vbo.bind();
        vertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        camera.movementSpeed = 0.5F;
    }

    auto update() -> void override
    {
        ImGui::Text("FPS %f", ImGui::GetIO().Framerate);
        const glm::mat4 resulted_matrix = getResultedViewMatrix();

        mandelbrotFractalShader.use();
        mandelbrotFractalShader.setMat4("projection", resulted_matrix);
        vertices.vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, vertices.vertices.size());
    }

    auto processInput() -> void override
    {
        Application2D::processInput();

        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS &&
            glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            const auto mode = glfwGetInputMode(window, GLFW_CURSOR);
            const double new_press_time = glfwGetTime();

            if (new_press_time - pressTime > 0.2) {
                pressTime = new_press_time;
            } else {
                return;
            }

            fmt::println("PRESSED");
            firstMouse = true;
            if (mode == GLFW_CURSOR_DISABLED) {
                isMouseShowed = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                isMouseShowed = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
    }

    auto onMouseRelativeMovement(const double delta_x, const double delta_y) -> void override
    {
        const auto scale = camera.zoom / 9000.0;

        camera.position += camera.up * static_cast<float>(delta_y * scale);
        camera.position += camera.right * static_cast<float>(delta_x * scale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = camera.zoom / 90.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }
};

auto main() -> int
{
    MandelbrotFractalApplication application{1000, 800, "Mandelbrot fractal"};
    application.run();
    return 0;
}
