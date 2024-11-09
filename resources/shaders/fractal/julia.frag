#version 330 core

in vec2 pointCoordinate;
in vec2 setC;
in float iterationsCount;

out vec4 FragColor;

vec2 mandelbrotF(vec2 value, vec2 c);
vec3 mandelbrotGetColor(float k);

void main()
{
    vec2 z = pointCoordinate;
    vec2 c = setC;

    for (float i = 0; i < iterationsCount; ++i) {
        z = mandelbrotF(z, c);

        if (length(z) > 2.0) {
            FragColor = vec4(mandelbrotGetColor(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.05, 0.05, 0.3, 1.0);
}