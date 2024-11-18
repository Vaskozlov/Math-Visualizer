#include <battery/embed.hpp>
#include <mv/application_2d.hpp>
#include <mv/gl/vertices_binder.hpp>
#include <mv/shader.hpp>
#include <mvl/mvl.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/io.hpp>

struct Cube
{
    glm::vec3 position;
    glm::vec4 color;
    double mass{};
    double velocity{};
    double acceleration{};
};

class PhysicsApplication : public mv::Application2D
{
private:
    std::array<char, 128> imguiWindowBufferTitle{};

    mv::Shader shader = mv::Shader{
        {b::embed<"resources/shaders/vertex.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    static constexpr float cubeSize = 0.5F;

    mv::gl::VerticesContainer<glm::vec3> cubeVertices{
        {cubeSize, cubeSize, 0.0F}, {cubeSize, -cubeSize, 0.0F},  {-cubeSize, -cubeSize, 0.0F},
        {cubeSize, cubeSize, 0.0F}, {-cubeSize, -cubeSize, 0.0F}, {-cubeSize, cubeSize, 0.0F},
    };


    std::string sourceCode =
        isl::io::read("/Users/vaskozlov/CLionProjects/Math-Visualizer/programs/physics.astlang");

    std::string programOutput = "";

    double pressTime = 0.0;
    ImFont *font;
    float fontScale = 0.33F;

    double initialTime = 0.0;
    bool running = false;

    bool disableInput = false;

    double mu = 0.1;
    double g = 9.8;
    double k = 400.0;

    std::array<Cube, 3> cubes = {
        Cube{{-3.0F, 0.0F, 0.0F}, {1.0F, 0.5F, 0.0F, 1.0F}, 1.0, 5.0, 0.0},
        Cube{{0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F, 1.0F}, 1.0, 0.0, 0.0},
        Cube{{3.0F, 0.0F, 0.0F}, {0.0F, 0.5F, 1.0F, 1.0F}, 2.0, 0.0, 0.0},
    };

public:
    using Application2D::Application2D;

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        ImGui::StyleColorsDark();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(30.0F);

        cubeVertices.loadData();

        cubeVertices.vbo.bind();
        cubeVertices.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        camera.position = glm::vec3(0.0F, 0.0F, 4.0F);

        cubes[0].acceleration = -mu * g;
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBufferTitle.data(), imguiWindowBufferTitle.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBufferTitle.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 3.0F, "%.3f");

        if (ImGui::Button("Start")) {
            running = true;
            initialTime = glfwGetTime();
        }

        ImGui::SameLine();

        if (ImGui::Button("Stop")) {
            running = false;
        }

        if (running) {
            auto dt = static_cast<double>(deltaTime);
            auto &[fi, se, th] = cubes;

            fi.position[0] += dt * fi.velocity;
            fi.velocity += dt * fi.acceleration;

            if (se.position[0] - (fi.position[0] + cubeSize) <= 0.0 ) {
                fi.acceleration = 0.0;
            }
        }

        shader.use();
        shader.setMat4("projection", getResultedViewMatrix());

        cubeVertices.vao.bind();

        for (size_t i = 0; i != cubes.size(); ++i) {
            shader.setVec4("elementColor", cubes[i].color);
            shader.setMat4("model", glm::translate(glm::mat4(1.0F), cubes[i].position));
            glDrawArrays(GL_TRIANGLES, 0, cubeVertices.vertices.size());
        }

        ImGui::PopFont();
        ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        if (!disableInput) {
            Application2D::processInput();
        }

        const auto left_alt_pressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
        const auto key_g_pressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

        if (left_alt_pressed && key_g_pressed) {
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

    auto onMouseRelativeMovement(const double delta_x, const double delta_y) -> void override
    {
        const auto scale = camera.zoom / 9000.0F;

        camera.position += camera.up * static_cast<float>(delta_y) * scale;
        camera.position += camera.right * static_cast<float>(delta_x) * scale;
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = static_cast<double>(camera.zoom) / 90.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }

    ~PhysicsApplication() override
    {
        isl::io::write(
            "/Users/vaskozlov/CLionProjects/Math-Visualizer/programs/physics.astlang", sourceCode);
    }
};

auto main() -> int
{
    PhysicsApplication application{1000, 800, "Fractal visualization", 2};
    application.run();
}
