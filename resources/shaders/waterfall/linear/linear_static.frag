#version 330 core

in vec3 vertexColor;
in vec2 valueLimits;
out vec4 FragColor;

vec3 floatToColor(float value, float min_value, float max_value);

void main()
{
    float value = vertexColor.g;

    if (isnan(value)) {
        FragColor = vec4(0);
        return;
    }

    if (isinf(value)) {
        FragColor = vec4(1.0, 1.0, 1.0, vertexColor.b);
        return;
    }

    FragColor = vec4(floatToColor(value, valueLimits.x, valueLimits.y), vertexColor.b);
}

