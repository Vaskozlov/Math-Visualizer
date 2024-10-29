#version 410 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform vec4 elementColor;

out vec4 vertexColor;

void main()
{
    gl_Position = projection * model * vec4(aPos.x, aPos.z, aPos.y, 1.0f);
    vec4 p = vec4(normalize(aPos) * 0.1, 0.1F);
    vertexColor = elementColor * 0.9 + p;
}