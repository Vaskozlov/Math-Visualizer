#ifndef MV_GL_INSTANCE_PARAMETERS_HPP
#define MV_GL_INSTANCE_PARAMETERS_HPP

#include <glm/glm.hpp>

namespace mv::gl
{
    struct InstanceParameters
    {
        glm::vec4 color;
        glm::mat4 transformation;
    };
}

#endif /* MV_GL_INSTANCE_PARAMETERS_HPP */
