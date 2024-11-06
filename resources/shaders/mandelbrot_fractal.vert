#version 330 core
layout(location = 0) in vec3 aPos;

out vec4 vertexColor;
out float iterationsCount;

uniform int iterations;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vertexColor = vec4(aPos, 1.0);
    iterationsCount = iterations;
}
