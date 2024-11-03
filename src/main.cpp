#include <battery/embed.hpp>
#include <mv/application.hpp>
#include <mv/gl/plot.hpp>
#include <mv/gl/shape/function.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/shapes_storage.hpp>
#include <mv/shader.hpp>

#include <imgui.h>

namespace math_1_3
{
    [[nodiscard]] constexpr auto fFunction(const glm::vec2 vec) -> float
    {
        return vec.x * vec.y * (6.0F - vec.x - vec.y);
    }

    [[nodiscard]] constexpr auto fFunctionGradient(const glm::vec2 vec) -> glm::vec2
    {
        return {
            6 * vec.y - 2 * vec.x * vec.y - vec.y * vec.y,
            6 * vec.x - vec.x * vec.x - 2 * vec.x * vec.y,
        };
    }
}// namespace math_1_3


class Application final : public mv::Application
{
private:
    mv::Shader shader = mv::Shader{
        b::embed<"resources/shaders/vertex.vert">().str(),
        b::embed<"resources/shaders/fragment.frag">().str(),
    };

    mv::Shader colorShader = mv::Shader{
        b::embed<"resources/shaders/colored_shader.vert">().str(),
        b::embed<"resources/shaders/fragment.frag">().str(),
    };

    mv::gl::shape::Sphere sphere{0.05F};

    mv::gl::shape::Plot plot{-10.0F, -10.0F, -50.0F, 10.0F, 10.0F, 50.0F};

    mv::gl::shape::Function function{
        math_1_3::fFunction,
        -4.0F,
        -4.0F,
        7.0F,
        7.0F,
    };

    std::vector<glm::vec3> points{};
    glm::vec3 extremum = {2.0F, 8.0F, 2.0F};
    double pressTime = 0.0;

public:
    using mv::Application::Application;

    auto init() -> void override
    {
        mv::Application::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        sphere.loadData();
        plot.mainAxis.loadData();
        plot.gird.loadData();
        function.loadData();

        sphere.bind(0, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
        plot.mainAxis.bind(0, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
        plot.gird.bind(0, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
        function.bind(0, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        glm::vec2 vec{6.0F};
        constexpr float a = 0.03F;

        for (uint32_t i = 1; i <= 30; ++i) {
            points.emplace_back(vec.x, math_1_3::fFunction(vec), vec.y);
            vec += math_1_3::fFunctionGradient(vec) * a;
        }
    }

    auto update() -> void override
    {
        glClearColor(0.8f, 0.8F, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            return;
        }

        ImGui::Text("FPS %f", ImGui::GetIO().Framerate);
        shader.use();

        const glm::mat4 resulted_matrix = getResultedViewMatrix();
        shader.setMat4("projection", resulted_matrix);

        shader.setVec4("elementColor", glm::vec4(0.1f, 0.1f, 0.4f, 1.0f));

        for (auto const &point : points) {
            sphere.drawAt(shader, point);
        }

        shader.setVec4("elementColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        sphere.drawAt(shader, extremum);

        shader.setVec4("elementColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        plot.mainAxis.drawAt(shader, glm::vec3{0.0F});

        shader.setVec4("elementColor", glm::vec4(0.07F, 0.07F, 0.07F, 1.0f));
        plot.gird.draw();

        colorShader.use();
        colorShader.setMat4("projection", resulted_matrix);
        colorShader.setVec4("elementColor", glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

        function.bindVao();
        constexpr auto model = glm::mat4(1.0f);

        colorShader.setMat4("model", model);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, function.vertices.size());
    }

    auto processInput() -> void override
    {
        mv::Application::processInput();

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
};

auto main() -> int
{
    Application application{1000, 800, "Hello World"};
    application.run();
    return 0;
}
