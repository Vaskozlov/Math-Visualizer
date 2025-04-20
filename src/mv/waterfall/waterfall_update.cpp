#include <glm/gtx/color_space.hpp>
#include <mv/waterfall_application.hpp>

namespace mv
{
    glm::vec3 hsvFloatToRgb(float value, float min_value, float max_value)
    {
        float v = std::clamp(value, min_value, max_value);
        float hue = 360.0F * (v - min_value) / (max_value - min_value);

        return glm::hsvColor(glm::vec3(hue, 1.0F, 1.0F));
    }

    auto Waterfall::resizeImages(const std::size_t width, const std::size_t height) -> void
    {
        waterfallWidth = width;
        waterfallHeight = height;

        std::size_t i = 0;

        for (; i < width / maxTextureSize; ++i) {
            if (powerWaterfalls.size() == i) {
                powerWaterfalls.emplace_back(maxTextureSize, height);
            } else {
                std::next(powerWaterfalls.begin(), i)->resize(maxTextureSize, height);
            }

            if (azimuthWaterfalls.size() == i) {
                azimuthWaterfalls.emplace_back(maxTextureSize, height);
            } else {
                std::next(azimuthWaterfalls.begin(), i)->resize(maxTextureSize, height);
            }
        }

        if (powerWaterfalls.size() == i) {
            powerWaterfalls.emplace_back(width % maxTextureSize, height);
        } else {
            std::next(powerWaterfalls.begin(), i)->resize(width % maxTextureSize, height);
        }

        if (azimuthWaterfalls.size() == i) {
            azimuthWaterfalls.emplace_back(width % maxTextureSize, height);
        } else {
            std::next(azimuthWaterfalls.begin(), i)->resize(width % maxTextureSize, height);
        }

        drawDetections();
    }

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

        auto camera_vec = camera.getPosition();

        const auto offset_width_scale = 2.0F / static_cast<float>(azimuthWaterfalls.size());

        const auto frequency_scale = static_cast<float>(
            2.0
            / (frequencyScale * static_cast<double>(azimuthWaterfalls.size() * maxTextureSize)));

        frequencyPosition = (1.0F + camera_vec.x) / frequency_scale / 1e3F;

        ImGui::Text(
            "x-axis: %.0f\ny-axis: %.0f",
            frequencyPosition,
            camera_vec.y * waterfallHeight * timeScale / imageHeightScale);

        ImGui::SliderFloat("Font scale", &fontScale, 0.2, 1.5);

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

        if (ImGui::SliderFloat(
                "Image width scale", &imageWidthScale, minWidthScale, maxWidthScale)) {
            drawDetections();
        }

        if (ImGui::SliderFloat(
                "Image height scale", &imageHeightScale, minHeightScale, maxHeightScale)) {
            drawDetections();
        }

        if (ImGui::SliderFloat(
                "Camera position",
                &frequencyPosition,
                0.0F,
                waterfallWidth * frequencyScale / 1e3F,
                "%.0f")) {
            camera_vec.x = frequencyPosition * frequency_scale * 1e3F - 1.0F;
            camera.setPosition(camera_vec);
        }

        if (ImGui::Button("Continue")) {
            continueFlag.test_and_set();
            continueFlag.notify_one();
        }

        ImGui::SameLine();

        if (ImGui::Button("Center")) {
            camera.setPosition({0.0F, 0.0F, 1.0F});
        }

        if (ImGui::Checkbox("Azimuth points", &showAzimuthPoints)) {
            showPowerPoints = !showPowerPoints;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Power points", &showPowerPoints)) {
            showAzimuthPoints = !showAzimuthPoints;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Azimuth detections", &showDetectionAzimuth)) {
            showDetectionPower = !showDetectionPower;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Power detections", &showDetectionPower)) {
            showDetectionAzimuth = !showDetectionAzimuth;
        }

        const auto projection =
            getCameraProjection(imageWidthScale, imageHeightScale) * getCameraView();

        waterfallShaderHsvF32->use();
        waterfallShaderHsvF32->setMat4("projection", projection);

        float index = 0.0F;

        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.bind(GL_TEXTURE0);

            azimuthMapSize.vao.bind();

            const auto real_width_scale = offset_width_scale
                                          * static_cast<float>(waterfall.getWidth())
                                          / static_cast<float>(maxTextureSize);

            const auto real_height_scale = imageHeightScale;

            auto trans = glm::mat4(1.0F);
            trans = glm::translate(trans, {index * offset_width_scale - 1.0F, 0.0F, 0.0F});
            trans = glm::scale(trans, {real_width_scale, real_height_scale, 1.0F});

            waterfallShaderHsvF32->setMat4("model", trans);
            azimuthMapSize.draw();
            index += 1.0F;
        }

        shaderHsvWithModel->use();
        shaderHsvWithModel->setMat4("projection", projection);

        rectangle.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_FAN, 0, rectangle.vertices.size(), rectangleInstances.models.size());

        ImGui::PopFont();
        ImGui::End();
    }

    auto Waterfall::fill(const float value) const -> void
    {
        for (const auto &waterfall : azimuthWaterfalls) {
            waterfall.fill(static_cast<gl::float16>(value));

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }

        for (const auto &waterfall : powerWaterfalls) {
            waterfall.fill(static_cast<gl::float16>(value));

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }
    }

    auto Waterfall::reloadImages() const -> void
    {
        for (const auto &waterfall : azimuthWaterfalls) {
            waterfall.reload();
        }

        for (const auto &waterfall : powerWaterfalls) {
            waterfall.reload();
        }

        azimuthWaterfallMask.reload();
        powerWaterfallMask.reload();
    }

    auto Waterfall::setPixel(
        const std::size_t x, const std::size_t y, const gl::float16 azimuth,
        const gl::float16 power) const -> void
    {
        std::next(azimuthWaterfalls.begin(), x / maxTextureSize)
            ->setPixelValue(x % maxTextureSize, y, azimuth);

        std::next(powerWaterfalls.begin(), x / maxTextureSize)
            ->setPixelValue(x % maxTextureSize, y, power);
    }

    auto Waterfall::drawDetections() -> void
    {
        rectangleInstances.models.clear();
        rectangleInstances.models.reserve(detections.size());

        const auto frequency_scale = 2.0F / static_cast<float>(frequencyScale)
                                     / static_cast<float>(powerWaterfalls.size() * maxTextureSize);

        for (const auto &detection : detections) {
            auto trans = glm::mat4(1.0F);

            trans = glm::translate(
                trans,
                {
                    static_cast<float>(detection.x) * frequency_scale - 1.0F,
                    static_cast<float>(detection.y)
                        / static_cast<float>(waterfallHeight * timeScale) * imageHeightScale,
                    0.0001F,
                });

            trans = glm::scale(
                trans,
                {
                    static_cast<float>(detection.width) * frequency_scale,
                    static_cast<float>(detection.height)
                        / static_cast<float>(waterfallHeight * timeScale) * imageHeightScale,
                    1.0F,
                });

            rectangleInstances.models.emplace_back(
                glm::vec4{
                    detection.azimuth,
                    0.3F,
                    0.0F,
                    0.0F,
                },
                trans);
        }

        rectangleInstances.loadData();
    }

} // namespace mv
