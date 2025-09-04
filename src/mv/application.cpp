#include <algorithm>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <mv/application.hpp>
#include <mv/gl/gl_init.hpp>
#include <mv/glfw/callbacks.hpp>
#include <mv/glfw/glfw_init.hpp>
#include <thread>

#ifdef __EMSCRIPTEN__
#    include <GLFW/emscripten_glfw3.h>
#    include <emscripten/emscripten.h>
#    include <emscripten/html5.h>
#endif

#include <GLFW/glfw3.h>

static mv::Application *application{};

struct FilesAccumulator {
    std::vector<std::filesystem::path> buffer;

    auto add(std::filesystem::path path) -> void {
        buffer.emplace_back(std::move(path));
    }
};

static FilesAccumulator DroppedFilesAccumulator;

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
void setupCanvas()
{
    EM_ASM({ Module.canvas = Module.canvas || document.getElementById('canvas'); });
}

EMSCRIPTEN_KEEPALIVE
extern "C" auto on_file_dropped(const char *filename) -> void
{
    DroppedFilesAccumulator.add(filename);
}

EMSCRIPTEN_KEEPALIVE
extern "C" auto files_dropped() -> void {
    application->onDrop(DroppedFilesAccumulator.buffer);
    DroppedFilesAccumulator.buffer.clear();
}

EM_JS(void, setup_drag_and_copy, (), {
    const canvas = document.getElementById('canvas');

    if (!canvas) {
        console.error('Canvas element not found');
        return;
    }

    canvas.addEventListener("dragover", (e) => {
        e.preventDefault();
    });

    canvas.addEventListener("drop", function (e) {
        e.preventDefault();

        const items = e.dataTransfer.items;
        const files = e.dataTransfer.files;

        function clearDirectory(path) {
            if (!FS.analyzePath(path).exists) return;

            const entries = FS.readdir(path);
            for (const name of entries) {
                if (name === "." || name === "..") continue;

                const fullPath = path + "/" + name;
                const stat = FS.stat(fullPath);

                if (FS.isDir(stat.mode)) {
                    clearDirectory(fullPath);
                    FS.rmdir(fullPath);
                } else {
                    FS.unlink(fullPath);
                }
            }
        }

        // Helper: Create nested directories
        function ensureDirectory(path) {
            const parts = path.split('/');
            let current = "";
            for (const part of parts) {
                if (!part) continue;
                current += '/' + part;

                if (!FS.analyzePath(current).exists) {
                    FS.mkdir(current);
                }
            }
        }

        function traverseFileTree(item, path = "") {
            return new Promise((resolve) => {
                if (item.isFile) {
                    item.file((file) => {
                        file.fullPath = path + file.name;
                        resolve([file]);
                    });
                } else if (item.isDirectory) {
                    const dirReader = item.createReader();
                    dirReader.readEntries(async (entries) => {
                        const results = await Promise.all(
                            entries.map((entry) =>
                                traverseFileTree(entry, path + item.name + "/")
                            )
                        );
                        resolve(results.flat());
                    });
                } else {
                    resolve([]);
                }
            });
        }

        (async () => {
            clearDirectory("/.drop");

            const collectedFiles = [];

            for (let i = 0; i < items.length; i++) {
                const entry = items[i].webkitGetAsEntry?.();
                if (entry) {
                    const files = await traverseFileTree(entry);
                    collectedFiles.push(...files);
                }
            }

            for (const file of collectedFiles) {
                const arrayBuffer = await file.arrayBuffer();
                const data = new Uint8Array(arrayBuffer);
                const path = "/.drop/" + file.fullPath;

                // Ensure directory exists
                const dirPath = path.split("/").slice(0, -1).join("/");
                ensureDirectory(dirPath);

                // Write to virtual FS
                FS.writeFile(path, data);
            }

            for (let i = 0; i < files.length; i++) {
                const file = files[i];
                const path = "/.drop/" + file.name;

                const len = lengthBytesUTF8(path) + 1;
                const pathPtr = _malloc(len);

                stringToUTF8(path, pathPtr, len);

                const isWasm64 = typeof Module.HEAPU64 !== 'undefined';

                Module.ccall("on_file_dropped", null, ["number"], [
                    isWasm64 ? BigInt(pathPtr) : pathPtr
                ]);

                _free(pathPtr);
            }

            Module.ccall("files_dropped", null, [], []);
        })();
    });
});
#endif

namespace mv
{
    auto findOutResourcesPath([[maybe_unused]] const int argc, [[maybe_unused]] const char *argv[])
        -> std::string
    {
#ifdef __EMSCRIPTEN__
        return "/resources";
#else

        // NOLINTNEXTLINE
        const auto *env_app_dir = std::getenv("APPDIR");

        if (env_app_dir != nullptr) {
            return env_app_dir;
        }

        if (argc != 2) {
            return "";
        }

        return argv[1];
#endif
    }

    auto invokeLoop() -> void
    {
        application->loop();
    }

    auto Application::loadFont(const float font_size) const -> ImFont *
    {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = true;
        config.SizePixels = font_size * 2;

        auto *result = imguiIO->Fonts->AddFontFromFileTTF(
            (resourcesPath / "fonts" / "JetBrainsMono-Medium.ttf").string().c_str(),
            font_size,
            &config,
            imguiIO->Fonts->GetGlyphRangesCyrillic());

        imguiIO->FontDefault = result;
        imguiIO->Fonts->Build();

        return result;
    }

    Application::Application(
        std::filesystem::path programs_path, const int width, const int height,
        std::string window_title, const int multisampling_level)
      : title{std::move(window_title)}
      , windowWidth{static_cast<float>(width)}
      , windowHeight{static_cast<float>(height)}
      , resourcesPath(std::move(programs_path))
    {
#ifdef __EMSCRIPTEN__
        setupCanvas();
        emscripten_glfw_set_next_window_canvas_selector("#canvas");
        glfw::init(2, 0);

#else
        glfw::init(3, 3);
        glfwSwapInterval(1);
#endif

        glfwWindowHint(GLFW_SAMPLES, multisampling_level);

        // NOLINTNEXTLINE
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (window == nullptr) {
            fmt::println(stderr, "Failed to create GLFW window.");
            glfwTerminate();
            std::terminate();
        }

        glfwMakeContextCurrent(window);
        glfw::setupCallbacksForApplication(this);

        gl::init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imguiIO = &ImGui::GetIO();

        imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        imguiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        imguiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef __EMSCRIPTEN__
        glfwSetWindowAspectRatio(window, GLFW_DONT_CARE, GLFW_DONT_CARE);
        ImGui_ImplOpenGL3_Init("#version 300 es");
#else
        ImGui_ImplOpenGL3_Init("#version 330 core");
#endif
    }

    auto Application::update() -> void
    {
        while (pool.executeOneTask()) {
        }

        drawGUI();
    }

    auto Application::onResize(int width, int height) -> void
    {
        windowWidth = static_cast<float>(width);
        windowHeight = static_cast<float>(height);

        glViewport(0, 0, width, height);
    }

    auto Application::onMouseMovement(const double x_pos_in, const double y_pos_in) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        if (firstMouse) {
            lastMouseX = x_pos_in;
            lastMouseY = y_pos_in;
            firstMouse = false;
        }

#if __EMSCRIPTEN__

        if (!isMouseShowed) {
            onMouseRelativeMovement(x_pos_in, -y_pos_in);
            return;
        }

#endif

        const double x_offset = x_pos_in - lastMouseX;
        const double y_offset = lastMouseY - y_pos_in;

        lastMouseX = x_pos_in;
        lastMouseY = y_pos_in;

        onMouseRelativeMovement(x_offset, y_offset);
    }

    auto Application::onMouseRelativeMovement(const double delta_x, const double delta_y) -> void
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_TRUE) {
            camera.rotate(delta_x, delta_y);
            return;
        }

        camera.processMouseMovement(static_cast<float>(delta_x), static_cast<float>(delta_y));
    }

    auto Application::onScroll(const double /*x_offset*/, const double y_offset) -> void
    {
        if (!isInFocus || isMouseShowed) {
            return;
        }

        camera.processMouseScroll(static_cast<float>(y_offset));
    }

    auto Application::submit(isl::Task<> task) -> isl::AsyncTask<void>
    {
        return pool.async(std::move(task));
    }

    auto Application::loop() -> void
    {
        using namespace std::chrono_literals;

        constexpr static auto delay_if_iconified = 100ms;

#ifdef __EMSCRIPTEN__
        glfwPollEvents();
#else
        constexpr static auto delay_for_10_fps = 1.0 / 10.0;
        glfwWaitEventsTimeout(delay_for_10_fps);
#endif

        const auto current_time = static_cast<float>(glfwGetTime());
        deltaTime = current_time - lastFrameTime;
        lastFrameTime = current_time;

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            std::this_thread::sleep_for(delay_if_iconified);
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        processInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update();

        if (showImgui) {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }

    auto Application::run() -> void
    {
        init();
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        application = this;

#ifdef __EMSCRIPTEN__
        setup_drag_and_copy();
        emscripten_glfw_make_canvas_resizable(window, "window", nullptr);
        emscripten_set_main_loop(invokeLoop, 60, 1);
#else
        while (glfwWindowShouldClose(window) == GLFW_FALSE) {
            loop();
        }
#endif
    }

    auto Application::processInput() -> void
    {
        constexpr static auto key_press_delay = 0.2;

        const auto left_shift_pressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        const auto key_g_pressed = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;

        if (left_shift_pressed && key_g_pressed) {
            const auto mode = glfwGetInputMode(window, GLFW_CURSOR);
            const double new_press_time = glfwGetTime();

            if (new_press_time - cursorModePressTime < key_press_delay) {
                return;
            }

            cursorModePressTime = new_press_time;
            firstMouse = true;
            isMouseShowed = mode == GLFW_CURSOR_DISABLED;

            glfwSetInputMode(
                window, GLFW_CURSOR, isMouseShowed ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
    }

    auto Application::onLeaveOrEnter(const bool entered) -> void
    {
        isInFocus = entered;
        firstMouse = true;
    }

    auto Application::onMouseClick(int, int, int) -> void
    {}

    auto Application::onDrop(const std::vector<std::filesystem::path> &) -> void
    {}

    Application::~Application()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
} // namespace mv
