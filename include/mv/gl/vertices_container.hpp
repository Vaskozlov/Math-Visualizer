#ifndef MV_VBO_HPP
#define MV_VBO_HPP

#include <mv/gl/vao.hpp>
#include <mv/gl/vbo.hpp>
#include <vector>

namespace mv::gl
{
    template<typename T>
    class VerticesContainer
    {
    public:
        std::vector<T> vertices{};
        VBO vbo;
        VAO vao;

        VerticesContainer() = default;

        VerticesContainer(std::initializer_list<T> initial_points)
          : vertices(initial_points)
        {}

        auto loadData() const -> void
        {
            vbo.loadData(vertices.data(), vertices.size() * sizeof(T));
        }
    };
}// namespace mv::gl

#endif /* MV_VBO_HPP */
