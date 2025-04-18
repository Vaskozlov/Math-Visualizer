#include <mv/waterfall_application.hpp>

namespace mv
{
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

        const auto camera_vec = camera.getPosition();

        const auto offset_width_scale = 2.0F / static_cast<float>(powerWaterfalls.size());

        ImGui::Text(
            "x-axis: %f\ny-axis: %f",
            waterfallWidth * static_cast<float>(powerWaterfalls.size() * maxTextureSize)
                / waterfallWidth * (camera_vec.x + 2.0F) / 4.0F * frequencyScale,
            (camera_vec.y + 1.0F) / 2.0F * waterfallHeight * timeScale);

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

        ImGui::SameLine();

        if (ImGui::Button("Center")) {
            camera.setPosition({0.0F, 0.0F, 1.0F});
        }

        const auto projection =
            getCameraProjection(imageWidthScale, imageHeightScale) * getCameraView();

        waterfallShaderHsvF32->use();
        waterfallShaderHsvF32->setMat4("projection", projection);

        waterfallShaderLinearF32->setVec4(
            "imageLimits",
            {0.0F,
             0.0F,
             static_cast<float>(window_width), // / 2.0F,
             static_cast<float>(window_height)});

        float index = 0.0F;

        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.bind(GL_TEXTURE0);
            powerWaterfallMask.bind(GL_TEXTURE1);

            waterfallShaderHsvF32->setVec2("texOffset", {0, 0});

            azimuthMapSize.vao.bind();

            const auto real_width_scale = offset_width_scale
                                          * static_cast<float>(waterfall.getWidth())
                                          / static_cast<float>(maxTextureSize);

            const auto real_height_scale = imageHeightScale;

            auto trans = glm::mat4(1.0F);
            trans = glm::translate(trans, {index * offset_width_scale, 0.0F, 0.0F});
            trans = glm::scale(trans, {real_width_scale, real_height_scale, 1.0F});

            waterfallShaderLinearF32->setMat4("model", trans);
            azimuthMapSize.draw();
            index += 1.0F;
        }

        colorShader->use();
        colorShader->setMat4("projection", projection);

        rectangle.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_FAN, 0, rectangle.vertices.size(), rectangleInstances.models.size());

        ImGui::PopFont();
        ImGui::End();
    }

    auto Waterfall::fill(const float value) const -> void
    {
        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.fill(static_cast<gl::float16>(value));

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }

        for (auto &waterfall : powerWaterfalls) {
            waterfall.fill(static_cast<gl::float16>(value));

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }
    }

    auto Waterfall::reloadImages() const -> void
    {
        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.reload();
        }

        for (auto &waterfall : powerWaterfalls) {
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
            ->setPixelValue(x % maxTextureSize, y, azimuth);
    }

    auto Waterfall::drawDetections() -> void
    {
        rectangleInstances.models.clear();
        rectangleInstances.models.reserve(detections.size());

        const auto freqScale = 1.0F / static_cast<float>(waterfallWidth * frequencyScale)
                               * static_cast<float>(waterfallWidth)
                               / static_cast<float>(powerWaterfalls.size() * maxTextureSize);

        for (const auto &[x, y, width, height] : detections) {
            auto trans = glm::mat4(1.0F);

            trans = glm::translate(
                trans,
                {
                    static_cast<float>(x) * freqScale,
                    static_cast<float>(y) / static_cast<float>(waterfallHeight),
                    0.0001F,
                });

            trans = glm::scale(
                trans,
                {
                    static_cast<float>(width) * freqScale,
                    static_cast<float>(height) / static_cast<float>(waterfallHeight * timeScale),
                    1.0F,
                });

            rectangleInstances.models.emplace_back(mv::Color::RED, trans);
        }

        rectangleInstances.loadData();
    }

} // namespace mv
