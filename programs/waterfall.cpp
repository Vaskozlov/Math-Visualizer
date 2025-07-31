#include <ccl/runtime.hpp>
#include <isl/coroutine/task.hpp>
#include <isl/float16.hpp>
#include <limits>
#include <mv/waterfall_application.hpp>

static mv::Waterfall *currentApp = nullptr;
static std::size_t ScaleY = 1;
static std::size_t ScaleX = 500;

static constexpr std::size_t width = 30'000'000 / 500;

static std::mt19937_64 engine;
static std::normal_distribution<float> power_distribution(-10.0F, 10.0F);
static std::uniform_real_distribution<float> azimuth_distribution1(70.0F, 110.0F);
static std::uniform_real_distribution<float> azimuth_distribution2(180.0F, 210.0F);

static auto generateNoise() -> std::pair<float, float>
{
    return {azimuth_distribution1(engine), power_distribution(engine)};
}

auto testTask() -> isl::Task<>
{
    co_await currentApp->fill(std::numeric_limits<float>::quiet_NaN());
    co_return;
}

static auto worker() -> isl::Task<>
{
    auto nan = isl::fp32ToFp16(std::numeric_limits<float>::quiet_NaN());

    co_await currentApp->resizeImages(width, 100 / ScaleY, nan, nan);
    co_await currentApp->fill(std::numeric_limits<float>::quiet_NaN());

    for (std::size_t y = 0; y < 100 / ScaleY; ++y) {
        for (std::size_t x = 0; x < width * ScaleX; x += ScaleX) {
            auto [az, power] = generateNoise();

            if (x < width * ScaleX / 2) {
                currentApp->setPixel(
                    x, y, isl::fp32ToFp16(azimuth_distribution1(engine)), isl::fp32ToFp16(power));
            } else {
                currentApp->setPixel(
                    x, y, isl::fp32ToFp16(azimuth_distribution2(engine)), isl::fp32ToFp16(power));
            }
        }
    }

    co_await currentApp->reloadImages();
    co_return;
}

auto main(const int argc, const char *argv[]) -> int
{
    auto resources = mv::findOutResourcesPath(argc, argv);

    if (resources.empty()) {
        fmt::println("Unable to find out resources path");
        return EXIT_FAILURE;
    }

    mv::Waterfall application{resources, 1000, 800, "Waterfall", 2};

    application.frequencyScale = static_cast<double>(ScaleX);
    application.timeScale = static_cast<double>(ScaleY);

    currentApp = &application;

    ccl::runtime::getGlobalThreadPool().launch(worker());

    application.run();

    return 0;
}
