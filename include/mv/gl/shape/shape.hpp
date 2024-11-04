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
            vao.bind();
            doDraw();
            vao.unbind();
        }

        auto drawAt(
            const Shader &shader, const glm::vec3 location = {0.0F, 0.0F, 0.0F},
            const float angle = 0.0F, const glm::vec3 rotation_vec = {1.0F, 0.0F, 0.0F}) const
            -> void
        {
            shader.setMat4(
                "model",
                glm::rotate(glm::translate(glm::mat4(1.0f), location), angle, rotation_vec));
            draw();
        }

    private:
        virtual auto doDraw() const -> void = 0;
    };
}// namespace mv::gl::shape

#endif /* MV_SHAPE_HPP */
