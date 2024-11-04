#include <battery/embed.hpp>
#include <mv/application.hpp>
#include <mv/gl/plot.hpp>
#include <mv/gl/shape/function.hpp>
#include <mv/gl/shape/prism.hpp>
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
    constexpr static auto clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);

    mv::Shader shader = mv::Shader{
        b::embed<"resources/shaders/vertex.vert">().str(),
        b::embed<"resources/shaders/fragment.frag">().str(),
    };

    mv::Shader shaderWithPositioning = mv::Shader{
        b::embed<"resources/shaders/static_instance.vert">().str(),
        b::embed<"resources/shaders/fragment.frag">().str(),
    };

    mv::Shader colorShader = mv::Shader{
        b::embed<"resources/shaders/colored_shader.vert">().str(),
        b::embed<"resources/shaders/fragment.frag">().str(),
    };

    mv::gl::shape::Sphere sphere{0.1F};

    mv::gl::shape::Plot plot{20.0F};

    mv::gl::Instancing<mv::gl::InstanceParameters> instancing;

    mv::gl::Instancing<mv::gl::InstanceParameters> sphereInstancing;

    mv::gl::shape::Function function{
        math_1_3::fFunction, -4.0F, -4.0F, 7.0F, 7.0F,
    };

    std::vector<glm::vec3> points{};
    glm::vec3 extremum = {2.0F, 8.0F, 2.0F};
    double pressTime = 0.0;

public:
    using mv::Application::Application;

    auto init() -> void override
    {
        instancing.models = {mv::gl::InstanceParameters{
            .color = {1.0F, 0.0F, 0.0F, 1.0F},
            .transformation = glm::translate(glm::mat4{1.0F}, {2.5F, 2.0F, 8.0F}),
        }};

        mv::Application::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        function.loadData();
        sphere.loadData();
        plot.axis.loadData();

        function.vbo.bind();
        function.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        plot.axis.vbo.bind();
        plot.axis.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        glm::vec2 vec{6.0F};
        constexpr float a = 0.03F;

        sphereInstancing.models.emplace_back(
            glm::vec4(1.0F, 0.0F, 0.0F, 1.0F), glm::translate(glm::mat4{1.0F}, {2.0F, 8.0F, 2.0F}));

        for (uint32_t i = 1; i <= 30; ++i) {
            sphereInstancing.models.emplace_back(
                glm::vec4(0.0F, 0.0F, 0.3F, 1.0F),
                glm::translate(glm::mat4{1.0F}, {vec.x, math_1_3::fFunction(vec), vec.y}));

            vec += math_1_3::fFunctionGradient(vec) * a;
        }

        sphereInstancing.loadData();

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstancing.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        colorShader.use();
        colorShader.setMat4("model", glm::mat4(1.0f));
        colorShader.setVec4("elementColor", glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));
    }

    auto update() -> void override
    {
        const auto b = glfwGetTime();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Text("FPS %f", ImGui::GetIO().Framerate);
        const glm::mat4 resulted_matrix = getResultedViewMatrix();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", resulted_matrix);
        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLES, 0, sphere.vertices.size(), sphereInstancing.models.size());

        colorShader.use();

        colorShader.setVec4("elementColor", glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

        colorShader.setMat4("projection", resulted_matrix);
        function.draw();

        colorShader.setVec4("elementColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        plot.axis.vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, plot.axis.vertices.size());
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
