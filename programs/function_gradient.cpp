#include <battery/embed.hpp>
#include <mv/application_3d.hpp>
#include <mv/gl/instance_parameters.hpp>
#include <mv/gl/instancing.hpp>
#include <mv/gl/plot.hpp>
#include <mv/gl/shape/function.hpp>
#include <mv/gl/shape/prism.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/shader.hpp>

#include <ast-lang-2/interpreter/interpreter.hpp>
#include <mvl/mvl.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>

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

class FunctionGradientApplication final : public mv::Application3D
{
private:
    std::array<char, 128> imguiWindowBuffer{};

    mv::Shader shader = mv::Shader{
        {b::embed<"resources/shaders/vertex.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader shaderWithPositioning = mv::Shader{
        {b::embed<"resources/shaders/static_instance.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader colorShader = mv::Shader{
        {b::embed<"resources/shaders/colored_shader.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::gl::shape::Sphere sphere{0.1F};

    mv::gl::shape::Plot plot{10};

    mv::gl::InstancesHolder<mv::gl::InstanceParameters> instancing;

    mv::gl::InstancesHolder<mv::gl::InstanceParameters> sphereInstancing;

    mv::gl::shape::Function function{
        math_1_3::fFunction, -10.0F, -10.0F, -15.0F, 10.0F, 10.0F, 15.0F
    };

    std::vector<glm::vec3> points{};
    glm::vec3 extremum = {2.0F, 8.0F, 2.0F};
    glm::vec3 lastPoint = {0.1F, 0.2F, 0.0F};
    double pressTime = 0.0;
    float gradientA = 0.03F;
    float gradientK = 1.0F;
    float evaluationTimeNs = 0.0;
    float functionDeltaStop = 1e-4;
    float gradientDeltaStop = 1e-8;
    float fontScale = 0.33F;
    ImFont *font;

    std::string sourceCode = "var a = print(1);";
    std::string programOutput = "";
    bool disableInput = false;

public:
    using Application3D::Application3D;

    auto calculateGradientPoints() -> void
    {
        using namespace std::chrono_literals;

        glm::vec2 evaluation_point{6.0F};
        sphereInstancing.models.clear();

        sphereInstancing.models.emplace_back(
            glm::vec4(1.0F, 0.0F, 0.0F, 1.0F), glm::translate(glm::mat4{1.0F}, extremum));

        float a = gradientA;

        const auto time_begin = std::chrono::high_resolution_clock::now();

        auto current_point = 0.0F;
        auto gradient_vector = glm::vec2(0.0F);

        do {
            current_point = math_1_3::fFunction(evaluation_point);
            gradient_vector = math_1_3::fFunctionGradient(evaluation_point) * a;

            sphereInstancing.models.emplace_back(
                glm::vec4(0.0F, 0.0F, 0.3F, 1.0F),
                glm::translate(
                    glm::mat4{1.0F}, {evaluation_point.x, current_point, evaluation_point.y}));


            if (sphereInstancing.models.size() > 5000) {
                break;
            }

            evaluation_point += gradient_vector;

            a = std::max(0.001F, a / gradientK);
        } while (std::abs(current_point - 8.0F) > functionDeltaStop &&
                 glm::length(gradient_vector) > gradientDeltaStop);

        lastPoint = glm::vec3(evaluation_point.x, evaluation_point.y, current_point);

        const auto time_end = std::chrono::high_resolution_clock::now();
        evaluationTimeNs = (time_end - time_begin) / 1ns;

        sphereInstancing.loadData();
    }

    auto init() -> void override
    {
        Application3D::init();

        setClearColor({0.8F, 0.8F, 0.8F, 1.0F});

        instancing.models = {mv::gl::InstanceParameters{
            .color = {1.0F, 0.0F, 0.0F, 1.0F},
            .transformation = glm::translate(glm::mat4{1.0F}, {2.5F, 2.0F, 8.0F}),
        }};

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        function.loadData();
        sphere.loadData();
        plot.loadData();

        function.vbo.bind();
        function.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        calculateGradientPoints();

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphereInstancing.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        colorShader.use();
        colorShader.setMat4("model", glm::mat4(1.0f));
        colorShader.setVec4("elementColor", glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

        ImGui::StyleColorsLight();
        font = loadFont<"resources/fonts/JetBrainsMono-Medium.ttf">(45.0F);

        camera.position = glm::vec3(0.0F, 12.0F, 10.0F);
    }

    auto update() -> void override
    {
        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Settings. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::Text(
            "Points evaluation time: %.0f ns, number of points: %zu",
            evaluationTimeNs,
            sphereInstancing.models.size() - 1);

        ImGui::Text(
            "Last evaluated point: (%.3f, %.3f, %.5f), vector length from extremum: %.3e",
            lastPoint.x,
            lastPoint.y,
            lastPoint.z,
            glm::length(lastPoint - glm::vec3(2.0F, 2.0F, 8.0F)));

        if (ImGui::SliderFloat("Gradiant a", &gradientA, 0.0F, 0.1F, "%.4f")) {
            calculateGradientPoints();
        }

        if (ImGui::SliderFloat("Gradiant k", &gradientK, 1.0F, 5.0F, "%.4f")) {
            calculateGradientPoints();
        }

        if (ImGui::SliderFloat("Function delta", &functionDeltaStop, 0.0F, 1e-3F, "%.5e")) {
            calculateGradientPoints();
        }

        if (ImGui::SliderFloat("Gradient delta", &gradientDeltaStop, 0.0F, 1e-3F, "%.5e")) {
            calculateGradientPoints();
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 3.0F, "%.3f");

        const glm::mat4 resulted_matrix = getResultedViewMatrix();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", resulted_matrix);
        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), sphereInstancing.models.size());

        colorShader.use();

        colorShader.setVec4("elementColor", glm::vec4(0.5f, 0.5f, 0.0f, 1.0f));

        colorShader.setMat4("projection", resulted_matrix);
        function.draw();

        colorShader.setVec4("elementColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

        plot.draw();

        ImGui::PopFont();
        ImGui::End();

        // ImGui::Begin("Program");
        // ImGui::PushFont(font);
        // ImGui::SetWindowFontScale(fontScale);

        // ImGui::InputTextMultiline("##Program input", &sourceCode);

        // if (ImGui::Button("run")) {
        //     try {
        //         auto node = mvl::parse(sourceCode, "stdin");
        //         programOutput.clear();
        //         auto interpreter = mvl::newInterpreter(std::back_inserter(programOutput));
        //         node->compute(interpreter);
        //     } catch (const std::exception &e) {
        //         programOutput = e.what();
        //     }
        // }

        // disableInput = ImGui::IsWindowFocused();

        // ImGui::TextUnformatted(programOutput.c_str(), programOutput.c_str() + programOutput.size());
        // ImGui::PopFont();
        // ImGui::End();
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        if (!disableInput) {
            Application3D::processInput();
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
};

auto main() -> int
{
    FunctionGradientApplication application{1000, 800, "Function Gradient", 16};
    application.run();
    return 0;
}
