#include <battery/embed.hpp>
#include <ccl/runtime.hpp>
#include <complex>
#include <execution>
#include <fmt/ranges.h>
#include <future>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <isl/linalg/linspace.hpp>
#include <list>
#include <mv/application_2d.hpp>
#include <mv/application_3d.hpp>
#include <mv/color.hpp>
#include <mv/gl/axes_2d.hpp>
#include <mv/gl/instances_holder.hpp>
#include <mv/gl/shape/plot_2d.hpp>
#include <mv/gl/shape/sphere.hpp>
#include <mv/gl/vertices_container.hpp>
#include <mv/shader.hpp>
#include <mvl/mvl.hpp>
#include <numbers>
#include <valarray>

class SystemOfEquations final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static float epsilon = 1e-4F;
    constexpr static double plotEpsilon = 5e-2;
    constexpr static std::size_t maxIterations = 1000;

    constexpr static glm::vec3 defaultCameraPosition{0.0F, 0.0F, 10.0F};

    std::array<char, windowTitleBufferSize> imguiWindowBuffer{};

    mv::Shader colorShader = mv::Shader{
        {b::embed<"resources/shaders/colored_shader.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::Shader shaderWithPositioning = mv::Shader{
        {b::embed<"resources/shaders/static_instance.vert">().str()},
        {b::embed<"resources/shaders/fragment.frag">().str()},
    };

    mv::gl::shape::Axes2D plot{12, 0.009F};

    mv::gl::shape::Sphere sphere{1.0F};
    mv::gl::shape::Sphere plotSphere{1.0F};
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> iterationsSpheres;
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> plotSpheres;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1500);
    std::valarray<float> functionY = functionX;

    float sphereRadius = 0.1F;

    std::size_t rightRootIterations = 0;
    float rightRoot = 0.0F;
    float rightRootDelta = 0.0F;

    std::size_t leftRootIterations = 0;
    float leftRoot = 0.0F;
    float leftRootDelta = 0.0F;

    std::size_t middleRootIterations = 0;
    float middleRoot = 0.0F;
    float middleRootDelta = 0.0F;

    std::list<mv::gl::shape::Plot2D> rightRootsLines;

    std::string input;

    float leftBorder = -5.0F;
    float rightBorder = 5.0F;

    ccl::parser::ast::SharedNode<mvl::ast::MathNode> upperEquation;
    ccl::parser::ast::SharedNode<mvl::ast::MathNode> lowerEquation;

    std::string upperInput{"tg(x * y + 0.3) - x^2"};
    std::string lowerInput{"0.5 * x^2 +2 * y^2 - 1"};

    float startX = 1.0F;
    float startY = 1.0F;

    std::size_t iterations = 0;
    float answerX = 0.0F;
    float answerY = 0.0F;

    auto addSphere(
        std::vector<mv::gl::InstanceParameters> &container, const mv::Color color,
        const glm::vec3 position, const float scale = 1.0F) const -> void
    {
        container.emplace_back(
            color,
            glm::scale(glm::translate(glm::mat4(1.0F), position), glm::vec3{sphereRadius * scale}));
    }

public:
    using Application2D::Application2D;

    auto compute(const float start_x, const float start_y) -> isl::Task<>
    {
        std::vector<mv::gl::InstanceParameters> new_spheres;


        auto x = start_x;
        auto y = start_y;

        auto prev_dx = std::numeric_limits<float>::infinity();
        auto prev_dy = std::numeric_limits<float>::infinity();

        for (iterations = 0; iterations < maxIterations; ++iterations) {
            auto a = upperEquation->derivationX(x, y);
            auto b = upperEquation->derivationY(x, y);

            auto c = lowerEquation->derivationX(x, y);
            auto d = lowerEquation->derivationY(x, y);

            auto f = upperEquation->compute(x, y);
            auto g = lowerEquation->compute(x, y);

            auto dy = (-f + a / c * g) / (b - a / c * d);
            auto dx = (-f - b * dy) / a;

            x += static_cast<float>(dx);
            y += static_cast<float>(dy);

            addSphere(new_spheres, mv::Color::RED, {x, y, 0.01F}, 0.8F);

            if (std::abs(dx - prev_dx) < epsilon && std::abs(dy - prev_dy) < epsilon) {
                break;
            }

            prev_dx = dx;
            prev_dy = dy;
        }

        answerX = x;
        answerY = y;

        addSphere(new_spheres, mv::Color::FOREST, {answerX, answerY, 0.01F});

        submit([this, spheres = std::move(new_spheres)]() mutable {
            iterationsSpheres.models = std::move(spheres);
            iterationsSpheres.loadData();
        });

        co_return;
    }

    auto drawPlot() -> isl::Task<>
    {
        std::vector<mv::gl::InstanceParameters> new_spheres;
        const auto begin = std::chrono::high_resolution_clock::now();

        for (std::size_t i = 0; i != functionX.size(); ++i) {
            for (std::size_t j = 0; j != functionY.size(); ++j) {
                if (std::abs(upperEquation->compute(functionX[i], functionY[j])) < plotEpsilon) {
                    addSphere(
                        new_spheres, mv::Color::ORANGE,
                        {
                            functionX[i],
                            functionY[j],
                            0.01F,
                        },
                        0.4F);
                }

                if (std::abs(lowerEquation->compute(functionX[i], functionY[j])) < plotEpsilon) {
                    addSphere(
                        new_spheres, mv::Color::OLIVE,
                        {
                            functionX[i],
                            functionY[j],
                            0.01F,
                        },
                        0.4F);
                }
            }
        }

        const auto end = std::chrono::high_resolution_clock::now();

        using namespace std::chrono_literals;
        fmt::println("{}", (end - begin) / 1.0s);

        submit([this, points = std::move(new_spheres)]() mutable {
            plotSpheres.models = std::move(points);
            plotSpheres.loadData();
        });

        co_return;
    }

    auto init() -> void override
    {
        Application2D::init();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        camera.setPosition(defaultCameraPosition);

        plot.loadData();
        plot.vbo.bind();
        plot.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        sphere.vbo.bind();
        sphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        iterationsSpheres.vbo.bind();
        sphere.vao.bindInstanceParameters(1, 1);

        plotSphere.vbo.bind();
        plotSphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        plotSpheres.vbo.bind();
        plotSphere.vao.bindInstanceParameters(1, 1);

        ImGui::StyleColorsLight();
        font = loadFont(30.0F);

        setClearColor(mv::Color::LIGHT_GRAY);
    }

    auto update() -> void override
    {
        Application2D::update();

        fmt::format_to_n(
            imguiWindowBuffer.data(), imguiWindowBuffer.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBuffer.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::Text(
            "Computed in %zu iterations, x = %f, y = %f\nStart point: (%f, %f)", iterations,
            answerX, answerY, startX, startY);

        ImGui::InputText("Upper equation", &upperInput);
        ImGui::InputText("Lower equation", &lowerInput);

        if (ImGui::Button("Draw")) {
            upperEquation = mvl::constructRoot(upperInput);
            lowerEquation = mvl::constructRoot(lowerInput);
            ccl::runtime::getGlobalThreadPool().launch(drawPlot());
        }

        ImGui::SameLine();

        if (ImGui::Button("Solve")) {
            upperEquation = mvl::constructRoot(upperInput);
            lowerEquation = mvl::constructRoot(lowerInput);

            ccl::runtime::getGlobalThreadPool().launch(compute(startX, startY));
        }

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);

        colorShader.setMat4(
            "model",
            glm::scale(
                glm::translate(glm::mat4(1.0f), {startX, startY, 0.0F}), glm::vec3(sphereRadius)));

        sphere.draw();

        colorShader.setVec4("elementColor", mv::Color::PINK);

        colorShader.setMat4(
            "model", glm::scale(glm::translate(glm::mat4(1.0f), {}), glm::vec3(sphereRadius)));

        sphere.draw();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        sphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), iterationsSpheres.models.size());

        plotSphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, sphere.vertices.size(), plotSpheres.models.size());

        if (ImGui::Button("Center camera")) {
            camera.setPosition(defaultCameraPosition);
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        ImGui::PopFont();
        ImGui::End();
    }

    glm::vec3 ScreenToWorldRay(double mouseX, double mouseY, int screenWidth, int screenHeight)
    {
        float x = (2.0f * mouseX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / screenHeight;// Flip Y-axis
        glm::vec4 nearPointNDC(x, y, -1.0f, 1.0f);
        glm::vec4 farPointNDC(x, y, 1.0f, 1.0f);

        glm::mat4 inverseVP = glm::inverse(getCameraProjection() * getCameraView());
        glm::vec4 nearPointWorld = inverseVP * nearPointNDC;
        glm::vec4 farPointWorld = inverseVP * farPointNDC;

        nearPointWorld /= nearPointWorld.w;
        farPointWorld /= farPointWorld.w;

        glm::vec3 rayOrigin = glm::vec3(nearPointWorld);
        glm::vec3 rayDirection = glm::normalize(glm::vec3(farPointWorld) - rayOrigin);

        return rayDirection;
    }

    // Function to get a 2D point along the ray (at z = 0 plane)
    glm::vec2 GetPointOn2DScene(glm::vec3 rayOrigin, glm::vec3 rayDirection, float zPlane = 0.0f)
    {
        float t = (zPlane - rayOrigin.z) / rayDirection.z;// Solve for t when z = zPlane
        glm::vec3 worldPoint = rayOrigin + t * rayDirection;
        return glm::vec2(worldPoint.x, worldPoint.y);// Return only x, y for 2D scene
    }

    auto onMouseClick(int button, int action, int mods) -> void override
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            glm::vec3 rayDirection = ScreenToWorldRay(mouseX, mouseY, width, height);
            glm::vec2 scenePoint = GetPointOn2DScene(
                camera.getPosition(), rayDirection, 0.0f);// Get 2D position on z = 0

            startX = scenePoint.x;
            startY = scenePoint.y;
        }
    }

    auto processInput() -> void override
    {
        constexpr static auto key_press_delay = 0.2;

        Application2D::processInput();

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

    auto onMouseRelativeMovement(const double delta_x, const double delta_y) -> void override
    {
        const auto scale = camera.getZoom() / 9000.0F;

        camera.relativeMove(camera.getUp() * static_cast<float>(delta_y) * scale);
        camera.relativeMove(camera.getRight() * static_cast<float>(delta_x) * scale);
    }

    auto onScroll(const double x_offset, const double y_offset) -> void override
    {
        const auto scale = static_cast<double>(camera.getZoom()) / 180.0;
        Application2D::onScroll(x_offset * scale, y_offset * scale);
    }
};

auto main() -> int
{
    SystemOfEquations application{1000, 800, "System of equations", 2};
    application.run();
    return 0;
}
