#version 330 core

in vec4 vertexColor;
in float iterationsCount;
out vec4 FragColor;

vec2 squareComplex(vec2 value)
{
    return vec2(value.x * value.x - value.y * value.y, 2.0 * value.x * value.y);
}

vec3 getColor(float k)
{
    if (k < 0.33) {
        return vec3(0.6, 0.5, 0.3) * (k / 0.25);
    }

    if (k < 0.66) {
        return vec3(0.3, 0.0, 0.6) * (k / 0.33 - 1.0);
    }

    return vec3(0.05, 0.05, 0.3) * (k / 0.33 - 2.0);
}

void main()
{
    vec2 c = vec2(vertexColor.x, vertexColor.y);
    vec2 z = vec2(0.0, 0.0);

    float teta = atan(c.y, c.x - 0.25);
    float rho_c = 0.5 - 0.5 * cos(teta);

    float x_without_quater = c.x - 0.25;
    float rho = sqrt(x_without_quater * x_without_quater + c.y * c.y);

    if (rho <= rho_c) {
        FragColor = vec4(0.05, 0.05, 0.3, 1.0);
        return;
    }

    for (float i = 0; i < iterationsCount; ++i) {
        z = squareComplex(z) + c;

        if (length(z) > 2.0) {
            FragColor = vec4(getColor(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.05, 0.05, 0.3, 1.0);
}