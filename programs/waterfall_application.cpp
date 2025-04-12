#include <mv/waterfall_application.hpp>

static mv::Waterfall *currentApp = nullptr;
static constexpr std::size_t ScaleX = 30'000;
static constexpr std::size_t ScaleY = 1;

struct Rect : public mv::Rect
{
    Rect(
        const std::size_t x, const std::size_t y, const std::size_t width, const std::size_t height)
      : mv::Rect(x / ScaleX, y / ScaleY, width / ScaleX, height / ScaleY)
    {}
};

std::pair<float, float> generateNoise()
{
    static std::mt19937_64 engine;
    static std::normal_distribution<float> power_distribution(-10.0F, 10.0F);
    static std::uniform_real_distribution<float> azimuth_distribution(0.0F, 360.0F);

    return {azimuth_distribution(engine), power_distribution(engine)};
}

auto main() -> int
{
    mv::Waterfall application{1000, 800, "Waterfall", 2};
    currentApp = &application;

    std::thread th([]() {
        currentApp->waitForFlag();
        currentApp->submit([]() {
            currentApp->resizeImages(30'000'000 / ScaleX, 500);

            for (std::size_t y = 0; y < currentApp->getAzimuthWaterfall().getHeight(); ++y) {
                for (std::size_t x = 0; x < currentApp->getAzimuthWaterfall().getWidth(); ++x) {
                    auto [az, power] = generateNoise();
                    currentApp->getAzimuthWaterfall().setPixelValue(x, y, az);
                    currentApp->getPowerWaterfall().setPixelValue(x, y, power);
                }
            }

            currentApp->reloadImages();
        });

        std::vector<Rect> rects{
            {0,          0,  100'000,   120},
            {500'000,    40, 500'000,   10 },
            {10'000'000, 60, 2'000'000, 20 },
        };

        currentApp->waitForFlag();

        currentApp->submit([rects]() {
            for (const auto &rect : rects) {
                currentApp->getAzimuthWaterfall().fillRect(rect, 100);
                currentApp->getPowerWaterfall().fillRect(rect, 100);
                currentApp->drawRect(rect);
            }
            currentApp->reloadImages();
        });

        currentApp->waitForFlag();
        currentApp->submit([]() {
            currentApp->resizeImages(30'000'000 / ScaleX, 1000 / ScaleY);
            currentApp->reloadImages();
        });

        std::vector<Rect> rects2{
            {0,          500, 100'000,   120},
            {500'000,    540, 500'000,   10 },
            {10'000'000, 560, 2'000'000, 20 },
        };

        currentApp->submit([rects2]() {
            for (const auto &rect : rects2) {
                currentApp->getAzimuthWaterfall().fillRect(rect, 100);
                currentApp->getPowerWaterfall().fillRect(rect, 100);
                currentApp->drawRect(rect);
            }
            currentApp->reloadImages();
        });
    });

    application.run();
    th.join();

    return 0;
}
