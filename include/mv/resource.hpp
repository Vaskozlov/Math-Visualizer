#ifndef MV_RESOURCE_HPP
#define MV_RESOURCE_HPP

#include <cstdlib>
#include <filesystem>
#include <isl/io.hpp>
#include <memory>

namespace mv
{
    inline auto getResourceAsString(const std::string_view name) -> std::string
    {
        const auto programs_path = std::filesystem::path(std::getenv("APPDIR"));
        const auto resources_path = programs_path / "share" / "resources";

        return isl::io::read(resources_path / name);
    }

    inline auto getResourceAsRaw(const std::string_view name) -> std::pair<std::size_t, void *>
    {
        const auto programs_path = std::filesystem::path(std::getenv("APPDIR"));
        const auto resources_path = programs_path / "share" / "resources";

        auto result = isl::io::read(resources_path / name);

        auto buffer = std::make_unique_for_overwrite<char[]>(result.size());
        std::copy_n(result.data(), result.size(), buffer.get());

        return std::make_pair(result.size(), static_cast<void*>(buffer.release()));
    }
} // namespace mv

#endif /* MV_RESOURCE_HPP */
