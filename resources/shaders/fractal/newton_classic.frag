#version 330 core

in vec2 pointCoordinate;
in float iterationsCount;

out vec4 FragColor;

vec2 cx_div(vec2 a, vec2 b);
vec2 cx_sqr(vec2 value);
vec3 getColorExpScale(float k);
vec2 newtonClassicFunction(vec2 z);

void main()
{
    vec2 z = pointCoordinate;

    for (float i = 0; i < iterationsCount; ++i) {
        z = z - cx_div(newtonClassicFunction(z), 3.0 * cx_sqr(z));

        if (length(newtonClassicFunction(z)) < 0.2) {
            FragColor = vec4(getColorExpScale(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}