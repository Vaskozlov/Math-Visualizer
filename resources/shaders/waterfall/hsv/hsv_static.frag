#version 330 core

in vec2 vertexColor;
in vec2 valueLimits;
out vec4 FragColor;

vec3 hsvFloatToRgb(float value, float min_value, float max_value);

void main()
{
    float value = vertexColor.r;

    if (isnan(value)) {
        FragColor = vec4(0);
        return;
    }

    if (isinf(value)) {
        FragColor = vec4(1.0, 1.0, 1.0, vertexColor.g);
        return;
    }

    FragColor = vec4(hsvFloatToRgb(value, valueLimits.x, valueLimits.y), vertexColor.g);
}

