#ifndef MV_SHAPE_HPP
#define MV_SHAPE_HPP

#include <mv/gl/vertices_binder.hpp>
#include <mv/shader.hpp>

namespace mv::gl::shape
{
    class Shape : public VerticesContainer<glm::vec3>
    {
    public:
        auto draw() const -> void
        {
            bindVao();
            doDraw();
        }

        auto drawAt(const Shader &shader, const glm::vec3 location = {0.0F, 0.0F, 0.0F}) const -> void
        {
            shader.setMat4("model", glm::translate(glm::mat4(1.0f), location));
            draw();
        }

    private:
        virtual auto doDraw() const -> void = 0;
    };
}// namespace mv::gl::shape

#endif /* MV_SHAPE_HPP */
