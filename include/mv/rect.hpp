#ifndef MV_RECT_HPP
#define MV_RECT_HPP

#include <cstddef>

namespace mv
{
    struct Rect
    {
        std::size_t x;
        std::size_t y;
        std::size_t width;
        std::size_t height;
    };
} // namespace mv

#endif /* MV_RECT_HPP */
