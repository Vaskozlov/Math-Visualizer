#ifndef MV_FUNCTION_HPP
#define MV_FUNCTION_HPP

#include <functional>
#include <mv/gl/shape/detail/polygons_shape.hpp>

namespace mv::gl::shape
{
    class Function final : public detail::PolygonsShape
    {
    private:
        constexpr static auto step = 0.05F;

    public:
        Function() = default;

        Function(const std::function<float(glm::vec2)> &function, const float space_size)
          : Function(
                function, -space_size, -space_size, -space_size, space_size, space_size, space_size)
        {}

        Function(
            const std::function<float(glm::vec2)> &function, const float min_x, const float min_y,
            const float min_z, const float max_x, const float max_y, const float max_z)
        {
            evaluatePoints(function, min_x, min_y, min_z, max_x, max_y, max_z);
        }

        auto evaluatePoints(
            const std::function<float(glm::vec2)> &function, float min_x, float min_y, float min_z,
            float max_x, float max_y, float max_z) -> void;
    };
}// namespace mv::gl::shape

#endif /* MV_FUNCTION_HPP */
