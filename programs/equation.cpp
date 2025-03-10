#include <battery/embed.hpp>
#include <ccl/runtime.hpp>
#include <complex>
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

class Computation2 final : public mv::Application2D
{
private:
    constexpr static auto windowTitleBufferSize = 128;

    constexpr static float epsilon = 1e-5F;
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

    mv::gl::shape::Plot2D functionGraph;
    mv::gl::shape::Plot2D dftGraph;

    mv::gl::shape::Prism prism{0.03F, 20.0F, 4};

    mv::gl::shape::Sphere chordSphere{1.0F};
    mv::gl::shape::Sphere secantSphere{1.0F};
    mv::gl::shape::Sphere newtonSphere{1.0F};

    mv::gl::InstancesHolder<mv::gl::InstanceParameters> chordSpheres;
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> secantSpheres;
    mv::gl::InstancesHolder<mv::gl::InstanceParameters> newtonSpheres;

    ImFont *font;
    double pressTime = 0.0;
    float fontScale = 0.5F;

    std::valarray<float> functionX = isl::linalg::linspace<float>(-10, 10, 1000);
    std::valarray<float> functionY = functionX;

    float sphereRadius = 0.1F;

    std::list<mv::gl::shape::Plot2D> rightRootsLines;

    std::string input = "x^3-3.125*x^2-3.5*x+2.458";
    std::string tmpInput = input;

    float leftBorder = -5.0F;
    float rightBorder = 5.0F;

    ccl::parser::ast::SharedNode<mvl::ast::MathNode> root;

    std::size_t chordIterations{};
    std::size_t secantIterations{};
    std::size_t newtonIterations{};

    float chordResult{};
    float secantResult{};
    float newtonResult{};
    float lineThickness = 0.03F;

public:
    using Application2D::Application2D;

    [[nodiscard]] auto f(const double x) const -> double
    {
        if (root == nullptr) {
            return std::numeric_limits<double>::quiet_NaN();
        }

        return root->compute(x, 0.0);
    }

    auto compute() -> isl::Task<>
    {
        root = mvl::constructRoot(input);

        if (root != nullptr) {
            constructPlot().await();
        }

        co_return;
    }

    auto constructPlot() -> isl::Task<>
    {
        if (root == nullptr) {
            co_return;
        }

        auto chord = ccl::runtime::async(chordMethod());
        auto secant = ccl::runtime::async(secantMethod());
        auto newton = ccl::runtime::async(newtonMethod());
        auto new_y = functionX;

        for (std::size_t i = 0; i != new_y.size(); ++i) {
            new_y[i] = f(new_y[i]);
        }

        submit([this, y = std::move(new_y)]() mutable {
            functionY = std::move(y);
            functionGraph.fill(functionX, functionY, lineThickness);
            functionGraph.loadData();
        });

        co_await chord;
        co_await secant;
        co_await newton;

        co_return;
    }

    auto chordMethod() -> isl::Task<>
    {
        auto new_spheres = std::vector<mv::gl::InstanceParameters>{};

        auto left_border = leftBorder;
        auto right_border = rightBorder;
        std::size_t it = 0;

        float xi = left_border - (right_border - left_border) / (f(right_border) - f(left_border)) *
                                     f(left_border);

        while (std::abs(f(xi)) > epsilon && it < maxIterations) {
            new_spheres.emplace_back(
                mv::Color::LIGHT_GREEN,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            xi,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            auto left = f(leftBorder);
            auto center = f(xi);

            if (left * center < 0) {
                right_border = xi;
            } else {
                left_border = xi;
            }

            xi = left_border -
                 (right_border - left_border) / (f(right_border) - f(left_border)) * f(left_border);

            ++it;
        }


        submit([this, spheres = std::move(new_spheres), xi, it]() mutable {
            chordIterations = it;
            chordResult = xi;
            chordSpheres.models = std::move(spheres);
            chordSpheres.loadData();
        });

        co_return;
    }

    auto secantMethod() -> isl::Task<>
    {
        auto x0 = leftBorder;
        auto x1 = rightBorder;
        auto new_spheres = std::vector<mv::gl::InstanceParameters>{};

        std::size_t it = 0;

        while (std::abs(f(x1)) > epsilon && it < maxIterations) {
            auto new_x = x1 - f(x1) * (x1 - x0) / (f(x1) - f(x0));

            new_spheres.emplace_back(
                mv::Color::RED,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            new_x,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));

            x0 = x1;
            x1 = new_x;

            ++it;
        }

        submit([this, spheres = std::move(new_spheres), x1, it]() mutable {
            secantResult = x1;
            secantIterations = it;
            secantSpheres.models = std::move(spheres);
            secantSpheres.loadData();
        });

        co_return;
    }

    auto newtonMethod() -> isl::Task<>
    {
        auto xi = (leftBorder + rightBorder) / 2;
        auto new_spheres = std::vector<mv::gl::InstanceParameters>{};
        std::size_t it = 0;

        while (std::abs(f(xi)) > epsilon && it < maxIterations) {
            xi = xi - f(xi) / root->derivationX(xi, 0.0);
            ++it;

            new_spheres.emplace_back(
                mv::Color::NAVY,
                glm::scale(
                    glm::translate(
                        glm::mat4(1.0F),
                        {
                            xi,
                            0.0F,
                            0.01F,
                        }),
                    glm::vec3{sphereRadius}));
        }

        submit([this, spheres = std::move(new_spheres), xi, it]() mutable {
            newtonResult = xi;
            newtonIterations = it;
            newtonSpheres.models = std::move(spheres);
            newtonSpheres.loadData();
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

        prism.loadData();
        prism.vbo.bind();
        prism.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        functionGraph.loadData();
        functionGraph.vbo.bind();
        functionGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        dftGraph.loadData();
        dftGraph.vbo.bind();
        dftGraph.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        secantSphere.vbo.bind();
        secantSphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        secantSpheres.vbo.bind();
        secantSphere.vao.bindInstanceParameters(1, 1);

        chordSphere.vbo.bind();
        chordSphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        chordSpheres.vbo.bind();
        chordSphere.vao.bindInstanceParameters(1, 1);

        newtonSphere.vbo.bind();
        newtonSphere.vao.bind(0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

        newtonSpheres.vbo.bind();
        newtonSphere.vao.bindInstanceParameters(1, 1);

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
            "Chord Method iterations: %zu, x: %f, f(x): %.3e\n"
            "Secant method iteration: %zu, x: %f, f(x): %.3e\n"
            "Newton method iterations: %zu, x: %f, f(x): %.3e\n",
            chordIterations, chordResult, f(chordResult), secantIterations, secantResult,
            f(secantResult), newtonIterations, newtonResult, f(newtonResult));

        ImGui::InputText("Equation", &tmpInput);

        if (ImGui::Button("Solve")) {
            input = tmpInput;
            ccl::runtime::getGlobalThreadPool().launch(compute());
        }

        ImGui::SliderFloat("Left border", &leftBorder, -20.0F, rightBorder);
        ImGui::SliderFloat("Right border", &rightBorder, leftBorder, 20.0F);

        if (ImGui::SliderFloat("Line width", &lineThickness, 0.001F, 0.1F)) {
            functionGraph.fill(functionX, functionY, lineThickness);
            functionGraph.loadData();
        }

        colorShader.use();
        colorShader.setMat4("projection", getResultedViewMatrix());
        colorShader.setMat4(
            "model", glm::translate(glm::mat4(1.0F), glm::vec3{0.0F, 0.0F, -0.02F}));
        colorShader.setVec4("elementColor", mv::Color::BLACK);

        plot.draw();

        colorShader.setVec4("elementColor", mv::Color::FOREST);
        prism.drawAt(colorShader, {leftBorder, 0.0F, 0.0F});

        colorShader.setVec4("elementColor", mv::Color::NAVY);
        prism.drawAt(colorShader, {rightBorder, 0.0F, 0.0F});

        colorShader.setMat4("model", glm::mat4(1.0F));
        colorShader.setVec4("elementColor", mv::Color::DARK_ORANGE);
        functionGraph.draw();

        colorShader.setVec4("elementColor", mv::Color::PINK);
        colorShader.setMat4("model", glm::scale(glm::mat4(1.0F), glm::vec3{sphereRadius}));
        secantSphere.draw();

        shaderWithPositioning.use();
        shaderWithPositioning.setMat4("projection", getResultedViewMatrix());

        chordSphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, chordSphere.vertices.size(), chordSpheres.models.size());

        secantSphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, secantSphere.vertices.size(), secantSpheres.models.size());

        newtonSphere.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP, 0, newtonSphere.vertices.size(), newtonSpheres.models.size());

        if (ImGui::Button("Center camera")) {
            camera.setPosition(defaultCameraPosition);
        }

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 1.5F, "%.3f");

        ImGui::PopFont();
        ImGui::End();
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
    Computation2 application{1000, 800, "Computation2", 2};
    application.run();
    return 0;
}
