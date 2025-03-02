#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 projection;
uniform vec2 minMaxValue;
uniform vec2 texOffset;

out vec2 TexCoord;
out vec2 valueLimits;

void main()
{
    gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    TexCoord = aTexCoord + texOffset;
    valueLimits = minMaxValue;
}