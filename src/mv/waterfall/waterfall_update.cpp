#include <mv/waterfall_application.hpp>

namespace mv
{
    auto Waterfall::updateAzimuthUniform() const -> void
    {
        waterfallShaderHsvF32.use();
        waterfallShaderHsvF32.setVec2(
            "minMaxValue",
            glm::vec2{
                azimuthMiddle - azimuthSide,
                azimuthMiddle + azimuthSide,
            });

        shaderHsvWithModel.use();
        shaderHsvWithModel.setVec2(
            "minMaxValue",
            glm::vec2{
                azimuthMiddle - azimuthSide,
                azimuthMiddle + azimuthSide,
            });
    }

    auto Waterfall::updatePowerUniform() const -> void
    {
        waterfallShaderLinearF32.use();
        waterfallShaderLinearF32.setVec2(
            "minMaxValue",
            glm::vec2{
                powerLow,
                powerHigh,
            });

        shaderLinearWithModel.use();
        shaderLinearWithModel.setVec2(
            "minMaxValue",
            glm::vec2{
                powerLow,
                powerHigh,
            });
    }

    auto Waterfall::resizeToFit(
        const double max_frequency, const std::size_t max_time, const isl::float16 default_azimuth,
        const isl::float16 default_power) -> isl::AsyncTask<void>
    {
        return resizeImages(
            static_cast<std::size_t>(
                std::ceil((max_frequency - frequencyStartOffset) / frequencyScale)),
            static_cast<std::size_t>(
                std::ceil((static_cast<double>(max_time) - timeStartOffset) / timeScale))
                + 1,
            default_azimuth,
            default_power);
    }

    auto Waterfall::resizeImages(
        const std::size_t width,
        const std::size_t height,
        const isl::float16 default_azimuth,
        const isl::float16 default_power) -> isl::AsyncTask<void>
    {
        return submit(doResizeImages(width, height, default_azimuth, default_power));
    }

    auto Waterfall::doResizeImages(
        const std::size_t width, const std::size_t height, const isl::float16 default_azimuth,
        const isl::float16 default_power) -> isl::Task<>
    {
        if (width == waterfallWidth && height <= waterfallHeight) {
            co_return;
        }

        waterfallWidth = width;
        waterfallHeight = height;

        std::size_t i = 0;

        for (; i < width / maxTextureSize; ++i) {
            if (powerWaterfalls.size() == i) {
                powerWaterfalls.emplace_back(maxTextureSize, height, default_power);
                azimuthWaterfalls.emplace_back(maxTextureSize, height, default_azimuth);
            } else {
                std::next(powerWaterfalls.begin(), static_cast<std::ptrdiff_t>(i))
                    ->resize(maxTextureSize, height, default_power);
                std::next(azimuthWaterfalls.begin(), static_cast<std::ptrdiff_t>(i))
                    ->resize(maxTextureSize, height, default_azimuth);
            }
        }

        if (powerWaterfalls.size() == i) {
            powerWaterfalls.emplace_back(width % maxTextureSize, height, default_power);
            azimuthWaterfalls.emplace_back(width % maxTextureSize, height, default_azimuth);
        } else {
            std::next(powerWaterfalls.begin(), static_cast<std::ptrdiff_t>(i))
                ->resize(width % maxTextureSize, height, default_power);
            std::next(azimuthWaterfalls.begin(), static_cast<std::ptrdiff_t>(i))
                ->resize(width % maxTextureSize, height, default_azimuth);
        }

        drawDetections();
        co_return;
    }

    auto Waterfall::update() -> void
    {
        Application2D::update();

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

        frequencyPosition = (camera_vec.x - waterfallStart.x) / frequency_scale / 1e3F;

        timePosition = (camera_vec.y - waterfallStart.y) * timeScale / imageHeightScale
                       * static_cast<double>(waterfallHeight);

        const auto time = static_cast<int64_t>(static_cast<double>(timePosition) + timeStartOffset);

        fmt::format_to_n(
            timeFormattingBuffer1.data(),
            timeFormattingBuffer1.size(),
            "{:%Y-%m-%d %H:%M:%S}.{:03}",
            fmt::localtime(time / 1000),
            time % 1000);

        const glm::vec2 scenePoint = getPointOn2DScene();

        mouseWordX = scenePoint.x;
        mouseWordY = scenePoint.y;

        const auto mouseFrequency = (scenePoint.x - waterfallStart.x) / frequency_scale / 1e3F;
        const auto mouseTime = static_cast<int64_t>(
            (scenePoint.y - waterfallStart.y) * timeScale / imageHeightScale
                * static_cast<double>(waterfallHeight)
            + timeStartOffset);

        fmt::format_to_n(
            timeFormattingBuffer2.data(),
            timeFormattingBuffer2.size(),
            "{:%Y-%m-%d %H:%M:%S}.{:03}",
            fmt::localtime(mouseTime / 1000),
            mouseTime % 1000);

        ImGui::Text(
            "x-axis: %.0f\n"
            "y-axis: %.0f ms = %s\n"
            "mouse frequency: %.0f, time: %ld = %s",
            frequencyPosition,
            timePosition + timeStartOffset,
            timeFormattingBuffer1.data(),
            mouseFrequency,
            mouseTime,
            timeFormattingBuffer2.data());

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
                "Frequency position",
                &frequencyPosition,
                0.0F,
                waterfallWidth * frequencyScale / 1e3F,
                "%.0f")) {
            camera_vec.x = frequencyPosition * frequency_scale * 1e3F + waterfallStart.x;
            camera.setPosition(camera_vec);
        }

        if (ImGui::SliderFloat(
                "Timeline", &timePosition, 0.0F, timeScale * waterfallHeight, "%.0f")) {
            camera_vec.y = timePosition
                               / static_cast<float>(
                                   timeScale / static_cast<double>(imageHeightScale)
                                   * static_cast<double>(waterfallHeight))
                           + waterfallStart.y;
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

        ImGui::Checkbox("Azimuth points", &showAzimuthPoints);

        ImGui::SameLine();
        ImGui::Checkbox("Power points", &showPowerPoints);

        ImGui::SameLine();
        ImGui::Checkbox("Azimuth detections", &showDetectionAzimuth);

        ImGui::SameLine();
        ImGui::Checkbox("Power detections", &showDetectionPower);

        const auto projection = getResultedViewMatrix();

        if (showAzimuthPoints) {
            drawAzimuthWaterfalls(projection, offset_width_scale);
        }

        if (showPowerPoints) {
            drawPowerWaterfalls(projection, offset_width_scale);
        }

        if (showDetectionAzimuth) {
            drawAzimuthDetections(projection, offset_width_scale);
        }

        if (showDetectionPower) {
            drawPowerDetections(projection, offset_width_scale);
        }

        ImGui::PopFont();
        ImGui::End();
    }

    auto Waterfall::doFill(const float value) -> isl::Task<>
    {
        const auto v = static_cast<isl::float16>(value);

        for (auto &waterfall : azimuthWaterfalls) {
            waterfall.fill(v);

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }

        for (auto &waterfall : powerWaterfalls) {
            waterfall.fill(v);

            glBindTexture(GL_TEXTURE_2D, waterfall.getTextureId());
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value);
        }

        co_return;
    }

    auto Waterfall::fill(const float value) -> isl::AsyncTask<void>
    {
        return submit(doFill(value));
    }

    auto Waterfall::doReloadImages() const -> isl::Task<>
    {
        for (const auto &waterfall : azimuthWaterfalls) {
            waterfall.reload();
        }

        for (const auto &waterfall : powerWaterfalls) {
            waterfall.reload();
        }

        co_return;
    }

    auto Waterfall::reloadImages() -> isl::AsyncTask<void>
    {
        return submit(doReloadImages());
    }

    auto Waterfall::setPixel(
        std::size_t x, std::size_t y, const isl::float16 azimuth, const isl::float16 power) -> void
    {
        x = static_cast<std::size_t>(std::round(static_cast<double>(x) / frequencyScale));
        y = static_cast<std::size_t>(
            std::round((static_cast<double>(y) - timeStartOffset) / timeScale));

        std::next(azimuthWaterfalls.begin(), static_cast<std::ptrdiff_t>(x / maxTextureSize))
            ->setPixelValue(x % maxTextureSize, y, azimuth);

        std::next(powerWaterfalls.begin(), static_cast<std::ptrdiff_t>(x / maxTextureSize))
            ->setPixelValue(x % maxTextureSize, y, power);
    }

    auto Waterfall::setPixelAzimuth(std::size_t x, std::size_t y, const isl::float16 azimuth)
        -> void
    {
        x = static_cast<std::size_t>(std::round(static_cast<double>(x) / frequencyScale));
        y = static_cast<std::size_t>(
            std::round((static_cast<double>(y) - timeStartOffset) / timeScale));

        std::next(azimuthWaterfalls.begin(), static_cast<std::ptrdiff_t>(x / maxTextureSize))
            ->setPixelValue(x % maxTextureSize, y, azimuth);
    }

    auto Waterfall::setPixelPower(std::size_t x, std::size_t y, const isl::float16 power) -> void
    {
        x = static_cast<std::size_t>(std::round(static_cast<double>(x) / frequencyScale));
        y = static_cast<std::size_t>(
            std::round((static_cast<double>(y) - timeStartOffset) / timeScale));

        std::next(powerWaterfalls.begin(), static_cast<std::ptrdiff_t>(x / maxTextureSize))
            ->setPixelValue(x % maxTextureSize, y, power);
    }

    auto Waterfall::doClear() -> isl::Task<>
    {
        azimuthWaterfalls.clear();
        powerWaterfalls.clear();
        detections.clear();
        waterfallWidth = 0;
        waterfallHeight = 0;
        timeStartOffset = 0.0;
        frequencyStartOffset = 0.0;
        co_return;
    }

    auto Waterfall::drawDetections() -> void
    {
        rectangleInstances.models.clear();
        rectangleInstances.models.reserve(detections.size());

        const auto frequency_scale =
            2.0F
            / (static_cast<float>(frequencyScale)
               * static_cast<float>(powerWaterfalls.size() * maxTextureSize));

        float index = 0.0F;

        for (const auto &detection : detections) {
            auto trans = glm::mat4(1.0F);

            trans = glm::translate(
                trans,
                {
                    static_cast<float>(detection.x) * frequency_scale + waterfallStart.x,
                    static_cast<float>(static_cast<double>(detection.y) - timeStartOffset)
                            / static_cast<float>(waterfallHeight * timeScale) * imageHeightScale
                        + waterfallStart.y,
                    0.00001F + index,
                });

            trans = glm::scale(
                trans,
                {
                    static_cast<float>(detection.width) * frequency_scale,
                    static_cast<float>(detection.height)
                        / static_cast<float>(static_cast<double>(waterfallHeight) * timeScale)
                        * imageHeightScale,
                    1.0F,
                });

            rectangleInstances.models.emplace_back(
                glm::vec4{
                    detection.azimuth,
                    detection.power,
                    0.3F,
                    0.0F,
                },
                trans);
            index += 0.00001F;
        }

        rectangleInstances.loadData();
    }

    auto Waterfall::drawAzimuthWaterfalls(
        const glm::mat4 &projection, const float offset_width_scale) const -> void
    {
        waterfallShaderHsvF32.use();
        waterfallShaderHsvF32.setMat4("projection", projection);

        float index = 0.0F;

        for (const auto &waterfall : azimuthWaterfalls) {
            waterfall.bind();

            azimuthMapSize.vao.bind();

            const auto real_width_scale = offset_width_scale
                                          * static_cast<float>(waterfall.getWidth())
                                          / static_cast<float>(maxTextureSize);

            const auto real_height_scale = imageHeightScale;

            auto trans = glm::mat4(1.0F);
            trans = glm::translate(
                trans, {index * offset_width_scale + waterfallStart.x, waterfallStart.y, 0.0F});
            trans = glm::scale(trans, {real_width_scale, real_height_scale, 1.0F});

            waterfallShaderHsvF32.setMat4("model", trans);
            azimuthMapSize.draw();
            index += 1.0F;
        }
    }

    auto Waterfall::drawPowerWaterfalls(
        const glm::mat4 &projection, const float offset_width_scale) const -> void
    {
        waterfallShaderLinearF32.use();
        waterfallShaderLinearF32.setMat4("projection", projection);

        float index = 0.0F;

        for (auto &waterfall : powerWaterfalls) {
            waterfall.bind();

            azimuthMapSize.vao.bind();

            const auto real_width_scale = offset_width_scale
                                          * static_cast<float>(waterfall.getWidth())
                                          / static_cast<float>(maxTextureSize);

            const auto real_height_scale = imageHeightScale;

            auto trans = glm::mat4(1.0F);
            trans = glm::translate(
                trans, {index * offset_width_scale + waterfallStart.x, waterfallStart.y, 0.0F});
            trans = glm::scale(trans, {real_width_scale, real_height_scale, 1.0F});

            waterfallShaderLinearF32.setMat4("model", trans);
            azimuthMapSize.draw();
            index += 1.0F;
        }
    }

    auto Waterfall::drawAzimuthDetections(const glm::mat4 &projection, const float) const -> void
    {
        shaderHsvWithModel.use();
        shaderHsvWithModel.setMat4("projection", projection);

        rectangle.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_FAN, 0, rectangle.vertices.size(), rectangleInstances.models.size());
    }

    auto Waterfall::drawPowerDetections(const glm::mat4 &projection, const float) const -> void
    {
        shaderLinearWithModel.use();
        shaderLinearWithModel.setMat4("projection", projection);

        rectangle.vao.bind();

        glDrawArraysInstanced(
            GL_TRIANGLE_FAN, 0, rectangle.vertices.size(), rectangleInstances.models.size());
    }
} // namespace mv
