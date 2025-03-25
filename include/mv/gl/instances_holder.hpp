#ifndef MV_INSTANCING_HPP
#define MV_INSTANCING_HPP

#include <mv/gl/vbo.hpp>
#include <vector>

namespace mv::gl
{
    template <typename T>
    class InstancesHolder
    {
    public:
        std::vector<T> models;
        VBO vbo;

        auto loadData() const -> void
        {
            vbo.loadData(models.data(), sizeof(T) * models.size());
        }
    };
} // namespace mv::gl

#endif /* MV_INSTANCING_HPP */
