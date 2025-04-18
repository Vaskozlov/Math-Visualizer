#include <mv/waterfall.hpp>

namespace mv
{
    auto Waterfall::update() -> void
    {
        Application2D::update();

        pollTask();

        int window_width;
        int window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);

        fmt::format_to_n(
            imguiWindowBufferTitle.data(),
            imguiWindowBufferTitle.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle",
            ImGui::GetIO().Framerate);

        ImGui::Begin(imguiWindowBufferTitle.data());
        ImGui::PushFont(font);
        ImGui::SetWindowFontScale(fontScale);

        ImGui::SliderFloat("Font scale", &fontScale, 0.1F, 3.0F, "%.3f");
        ImGui::InputText("Lib path: ", &libraryPath);

        if (ImGui::Button("Load")) {
            shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reload")) {
            if (!shaderLib.hasLibrary()) {
                shaderLib = isl::SharedLibLoader::loadLibrary(libraryPath).value();
            } else {
                shaderLib.reload();
            }

            auto *function =
                reinterpret_cast<void (*)()>(shaderLib.getSymbol("print_hello_world").value());

            function();
        }

        if (ImGui::SliderFloat("Middle", &azimuthMiddle, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderFloat("Side", &azimuthSide, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderFloat("Low", &powerLow, minPower, maxPower)) {
            updatePowerUniform();
        }

        if (ImGui::SliderFloat("High", &powerHigh, minPower, maxPower)) {
            updatePowerUniform();
        }

        ImGui::SliderFloat("Image width scale", &imageWidthScale, minWidthScale, maxWidthScale);
        ImGui::SliderFloat("Image height scale", &imageHeightScale, minHeightScale, maxHeightScale);

        if (ImGui::Button("Continue")) {
            continueFlag.test_and_set();
            continueFlag.notify_one();
        }

        const auto camera_vec = camera.getPosition();

        azimuthWaterfall.bind(GL_TEXTURE0);
        azimuthWaterfallMask.bind(GL_TEXTURE1);

        waterfallShaderHsvF32->use();
        waterfallShaderLinearF32->setVec4(
            "imageLimits",
            {0.0F,
             0.0F,
             static_cast<float>(window_width) / 2.0F,
             static_cast<float>(window_height)});
        waterfallShaderHsvF32->setVec2("texOffset", {camera_vec.x, camera_vec.y});

        azimuthMapSize.vao.bind();
        waterfallShaderLinearF32->setMat4(
            "model",
            glm::scale(
                glm::translate(
                    glm::mat4(1.0f),
                    {-0.5F * imageWidthScale - 0.5F, -1.0F * imageHeightScale, 0.0F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        azimuthMapSize.draw();

        powerWaterfall.bind(GL_TEXTURE0);
        powerWaterfallMask.bind(GL_TEXTURE1);

        waterfallShaderLinearF32->use();
        waterfallShaderLinearF32->setVec4(
            "imageLimits",
            {static_cast<float>(window_width) / 2.0F,
             0.0F,
             static_cast<float>(window_width),
             static_cast<float>(window_height)});
        waterfallShaderLinearF32->setVec2("texOffset", {camera_vec.x, camera_vec.y});

        powerMapSize.vao.bind();
        waterfallShaderLinearF32->setMat4(
            "model",
            glm::scale(
                glm::translate(
                    glm::mat4(1.0f),
                    {-0.5F * imageWidthScale + 0.5F, -1.0F * imageHeightScale, 0.0F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        powerMapSize.draw();

        ImGui::PopFont();
        ImGui::End();
    }
} // namespace mv
