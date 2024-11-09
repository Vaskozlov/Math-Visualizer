#version 330 core

in vec2 pointCoordinate;
in float iterationsCount;

out vec4 FragColor;

vec2 cx_sqr(vec2 value);
vec2 cx_cube(vec2 z);
vec2 cx_inverse(vec2 z);
vec3 getColorNoScale(float k);
vec2 newtonClassicFunction(vec2 z);

void main()
{
    vec2 z = pointCoordinate;

    const vec2 p1 = vec2(-0.5, sqrt(3.0) / 2.0);
    const vec2 p2 = vec2(-0.5, -sqrt(3.0) / 2.0);

    for (float i = 0; i < iterationsCount; ++i) {
        z = z - (cx_inverse(cx_sqr(vec2(z.y, -z.x))) + z) / 3.0;
        vec2 t = newtonClassicFunction(z);

        float l1 = length(t - p1);
        float l2 = length(t - p2);

        if (l1 < 0.3 || l2 < 0.3) {
            FragColor = vec4(getColorNoScale(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}