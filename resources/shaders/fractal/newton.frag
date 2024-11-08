#version 330 core

in vec4 vertexColor;
in float iterationsCount;
out vec4 FragColor;

#define cx_mul(a, b) vec2(a.x *b.x - a.y * b.y, a.x * b.y + a.y * b.x)
#define cx_div(a, b)                                                                               \
    vec2(                                                                                          \
        ((a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y)),                                       \
        ((a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y)))

vec3 newtonGetColor(float k)
{
    if (k < 0.33) {
        return vec3(0.6, 0.5, 0.3);
    }

    if (k < 0.66) {
        return vec3(0.3, 0.0, 0.6);
    }

    return vec3(0.05, 0.05, 0.3);
}

vec2 f(vec2 value, vec2 c)
{
    return cx_mul(cx_mul(value, value), value) - 1.0;
}

void main()
{
    vec2 c = vec2(vertexColor.z, vertexColor.w);
    vec2 z = vec2(vertexColor.x, vertexColor.y);
    float i = 0;

    for (; i < iterationsCount; ++i) {
        z = z - cx_div(f(z, z), (3.0 * cx_mul(z, z)));

        vec2 t = f(z, z);

        float l1 = length(t - vec2(-0.5, sqrt(3.0) / 2.0));
        float l2 = length(t - vec2(-0.5, -sqrt(3.0) / 2.0));

        if (l1 < 0.3 || l2 < 0.3) {
            FragColor = vec4(newtonGetColor(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.3, 0.3, 0.3, 0.1);
}