#include <mv/gl/plot.hpp>

namespace mv::gl::shape
{
    Plot::Plot(
        const float min_x, const float min_y, const float min_z, const float max_x,
        const float max_y, const float max_z)
      : minX{min_x}
      , minY{min_y}
      , minZ{min_z}
      , maxX{max_x}
      , maxY{max_y}
      , maxZ{max_z}
    {
        mainAxis.vertices.reserve(8);

        mainAxis.vertices.emplace_back(minX, 0, 0);
        mainAxis.vertices.emplace_back(maxX, 0, 0);
        mainAxis.vertices.emplace_back(0, minY, 0);
        mainAxis.vertices.emplace_back(0, maxY, 0);
        mainAxis.vertices.emplace_back(0, 0, minZ);
        mainAxis.vertices.emplace_back(0, 0, maxZ);

        float x = floor(minX);
        float y = floor(minY);

        while (x <= maxX) {
            gird.vertices.emplace_back(x, minY, 0);
            gird.vertices.emplace_back(x, maxY, 0);
            x += 1.0F;
        }

        while (y <= maxY) {
            gird.vertices.emplace_back(minX, y, 0);
            gird.vertices.emplace_back(maxX, y, 0);
            y += 1.0F;
        }
    }
}// namespace mv::gl::shape