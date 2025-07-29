#ifndef MV_GL_INIT_HPP
#define MV_GL_INIT_HPP

#ifdef __EMSCRIPTEN__
#    include <GLES3/gl3.h>
#else
#    include <GL/glew.h>
#endif

namespace mv::gl
{
    void init();
} // namespace mv::gl

#endif /* MV_GL_INIT_HPP */
