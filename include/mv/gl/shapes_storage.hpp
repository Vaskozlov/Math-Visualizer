#ifndef MV_SHAPES_STORAGE_HPP
#define MV_SHAPES_STORAGE_HPP

#include <GL/glew.h>

#include <mv/gl/vertices_binder.hpp>

namespace mv::gl
{
    enum class ShapeType
    {
        TRIANGLE = GL_TRIANGLES,
        LINE = GL_LINES,
    };

    class ShapeStorage
    {
    private:
        VerticesContainer<glm::vec3> vertexBinder;
        ShapeType shapeType;

    public:
        ShapeStorage(const ShapeType type, VerticesContainer<glm::vec3> vertex_binder)
          : vertexBinder{std::move(vertex_binder)}
          , shapeType{type}
        {}
    };
}// namespace mv::gl

#endif /* MV_SHAPES_STORAGE_HPP */
