#include <mv/waterfall_application.hpp>

namespace mv
{
    auto Waterfall::update() -> void
    {
        pollTask();

        fmt::format_to_n(
            imguiWindowBufferTitle.data(), imguiWindowBufferTitle.size(),
            "Настройки. FPS: {:#.4}###SettingWindowTitle", ImGui::GetIO().Framerate);

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

        if (ImGui::SliderFloat("Azimuth middle", &azimuthMiddle, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderFloat("Azimuth side", &azimuthSide, azimuthMin, azimuthMax)) {
            updateAzimuthUniform();
        }

        if (ImGui::SliderInt("Power low", &powerLow, minPower, maxPower)) {
            updatePowerUniform();
        }

        if (ImGui::SliderInt("Power high", &powerHigh, minPower, maxPower)) {
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
        waterfallShaderHsvF32->setVec2("texOffset", glm::vec2(camera_vec.x, camera_vec.y) * 0.1F);

        azimuthMapSize.vao.bind();
        waterfallShaderLinearU32->setMat4(
            "model",
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3{-1.0F, -1.0F, 0.01F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        azimuthMapSize.draw();

        powerWaterfall.bind(GL_TEXTURE0);
        powerWaterfallMask.bind(GL_TEXTURE1);

        waterfallShaderLinearU32->use();
        waterfallShaderLinearU32->setVec2(
            "texOffset", glm::vec2(camera_vec.x, camera_vec.y) * 0.1F);

        powerMapSize.vao.bind();
        waterfallShaderLinearU32->setMat4(
            "model",
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3{0.0F, -1.0F, 0.00F}),
                {imageWidthScale, imageHeightScale * 2.0F, 1.0F}));
        powerMapSize.draw();

        ImGui::PopFont();
        ImGui::End();
    }

}// namespace mv
