#version 330 core
layout(location = 0) in vec3 aPos;

out vec2 pointCoordinate;
out vec2 setC;
out float iterationsCount;

uniform int iterations;
uniform vec2 fC;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    pointCoordinate = vec2(aPos);
    setC = fC;
    iterationsCount = iterations;
}
