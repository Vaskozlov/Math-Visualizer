#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 projection;
uniform vec2 minMaxValue;
uniform vec2 texOffset;
uniform vec4 imageLimits;

out vec2 TexCoord;
out vec2 valueLimits;
out vec4 imageMaxSize;

void main()
{
    gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    TexCoord = aTexCoord + texOffset;
    valueLimits = minMaxValue;
    imageMaxSize = imageLimits;
}