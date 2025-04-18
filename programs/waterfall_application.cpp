#include <mv/waterfall_application.hpp>

static mv::Waterfall *currentApp = nullptr;
static std::size_t ScaleY = 1;
static std::size_t ScaleX = 4'000;

static constexpr std::size_t width = 30'000'000 / 500;

static std::mt19937_64 engine;
static std::normal_distribution<float> power_distribution(-10.0F, 10.0F);
static std::uniform_real_distribution<float> azimuth_distribution1(70.0F, 110.0F);
static std::uniform_real_distribution<float> azimuth_distribution2(180.0F, 210.0F);

std::pair<float, float> generateNoise()
{
    return {azimuth_distribution1(engine), power_distribution(engine)};
}

auto main() -> int
{
    mv::Waterfall application{1000, 800, "Waterfall", 2};

    application.frequencyScale = 500.0;
    application.timeScale = 1.0;

    currentApp = &application;

    std::thread th([]() {
        currentApp->submit([]() {
            currentApp->resizeImages(width, 1000 / ScaleY);
            currentApp->fill(std::numeric_limits<float>::quiet_NaN());
            currentApp->fill(std::numeric_limits<float>::quiet_NaN());

            for (std::size_t y = 0; y < 1000 / ScaleY; ++y) {
                for (std::size_t x = 0; x < width; ++x) {
                    auto [az, power] = generateNoise();

                    if (x < width / 2) {
                        currentApp->setPixel(
                            x,
                            y,
                            static_cast<mv::gl::float16>(azimuth_distribution1(engine)),
                            static_cast<mv::gl::float16>(power));
                    } else {
                        currentApp->setPixel(
                            x,
                            y,
                            static_cast<mv::gl::float16>(azimuth_distribution2(engine)),
                            static_cast<mv::gl::float16>(power));
                    }
                }
            }

            currentApp->reloadImages();
        });

        std::vector<mv::Rect> rects{
            {0,          500, 100'000,    120 },
            {500'000,    540, 500'000,    10  },
            {10'000'000, 560, 10'000'000, 20  },
            {29'000'000, 0,   1'000'000,  1000},
        };

        for (const auto &rect : rects) {
            currentApp->addRect(rect);
        }

        currentApp->submit([]() { currentApp->drawDetections(); });

        currentApp->submit([]() {
            currentApp->resizeImages(width, 2000 / ScaleY);

            for (std::size_t y = 1000; y < 2000 / ScaleY; ++y) {
                for (std::size_t x = 0; x < width; ++x) {
                    auto [az, power] = generateNoise();

                    if (x < width / 2) {
                        currentApp->setPixel(
                            x,
                            y,
                            static_cast<mv::gl::float16>(azimuth_distribution1(engine)),
                            static_cast<mv::gl::float16>(power));
                    } else {
                        currentApp->setPixel(
                            x,
                            y,
                            static_cast<mv::gl::float16>(azimuth_distribution2(engine)),
                            static_cast<mv::gl::float16>(power));
                    }
                }
            }

            currentApp->reloadImages();
        });
    });

    application.run();
    th.join();

    return 0;
}
