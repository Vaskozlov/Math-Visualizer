#include <mv/waterfall_application.hpp>

static mv::Waterfall *currentApp = nullptr;

extern void setImagesSize(const std::size_t width, const std::size_t height)
{
    currentApp->pushCommand([width, height]() {
        currentApp->resizeImages(width, height);
    });
}

extern void drawRect(
    const std::size_t x, const std::size_t y, const std::size_t width, const std::size_t height)
{
    currentApp->pushCommand([x, y, width, height]() {
        currentApp->drawRect(x, y, width, height);
    });
}

extern void reloadImages()
{
    currentApp->pushCommand([]() {
        currentApp->reloadImages();
    });
}

extern auto waitForContinue() -> void
{
    currentApp->waitForFlag();
}

auto main() -> int
{
    mv::Waterfall application{1000, 800, "Waterfall", 2};
    currentApp = &application;

    std::thread th([]() {
        waitForContinue();

        setImagesSize(1024, 1024);
        waitForContinue();

        drawRect(400, 0, 30, 1024);

        waitForContinue();
        reloadImages();

        std::mt19937_64 generator;
        std::uniform_real_distribution<float> angle_distribution(0.0f, 360.0f);
        std::uniform_real_distribution<float> high_angle_distribution(10.0F, 34.0F);
        std::uniform_real_distribution<float> second_high_angle_distribution(40.0F, 50.0F);
        std::uniform_int_distribution<std::uint32_t> power_distribution(0, 40);

        std::uniform_int_distribution<std::uint32_t> high_power_distribution(100 / 2, 100);

        const auto &azimuthWaterfall = currentApp->getAzimuthWaterfall();
        const auto &powerWaterfall = currentApp->getPowerWaterfall();

        for (std::size_t i = 0; i < azimuthWaterfall.getHeight(); ++i) {
            for (std::size_t j = 0; j < azimuthWaterfall.getWidth(); ++j) {
                if (j > 400 && j < 430) {
                    if (j < 415) {
                        azimuthWaterfall.setPixelValue(j, i, high_angle_distribution(generator));
                    } else {
                        azimuthWaterfall.setPixelValue(
                            j, i, second_high_angle_distribution(generator));
                    }
                } else {
                    azimuthWaterfall.setPixelValue(j, i, angle_distribution(generator));
                }
            }
        }

        waitForContinue();
        reloadImages();

        for (std::size_t i = 0; i < powerWaterfall.getHeight(); ++i) {
            for (std::size_t j = 0; j < powerWaterfall.getWidth(); ++j) {
                if (j > 400 && j < 430) {
                    powerWaterfall.setPixelValue(j, i, high_power_distribution(generator));
                } else {
                    powerWaterfall.setPixelValue(j, i, power_distribution(generator));
                }
            }
        }

        waitForContinue();
        reloadImages();
    });

    application.run();
    th.join();

    return 0;
}
