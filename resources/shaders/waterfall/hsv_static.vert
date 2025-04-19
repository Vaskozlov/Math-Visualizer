#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in mat4 aTransformation;

uniform mat4 projection;
uniform vec2 minMaxValue;

out vec2 vertexColor;
out vec2 valueLimits;

void main()
{
    gl_Position = projection * aTransformation * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    vertexColor = vec2(aColor.x, aColor.y);
    valueLimits = minMaxValue;
}