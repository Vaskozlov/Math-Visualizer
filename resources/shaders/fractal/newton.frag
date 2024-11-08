#version 330 core

in vec4 vertexColor;
in float iterationsCount;
out vec4 FragColor;

#define cx_mul(a, b) vec2(a.x *b.x - a.y * b.y, a.x * b.y + a.y * b.x)
#define cx_div(a, b)                                                                               \
    vec2(                                                                                          \
        ((a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y)),                                       \
        ((a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y)))

#define cx_sin(a) vec2(sin(a.x) * cosh(a.y), cos(a.x) * sinh(a.y))
#define cx_cos(a) vec2(cos(a.x) * cosh(a.y), -sin(a.x) * sinh(a.y))

vec2 cx_sqr(vec2 value)
{
    return vec2(value.x * value.x - value.y * value.y, 2.0 * value.x * value.y);
}

vec2 cx_cube(vec2 z)
{
    return vec2(z.x * z.x * z.x - 3.0 * z.x * z.y * z.y, 3 * z.x * z.x * z.y - z.y * z.y * z.y);
}

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

vec2 f(vec2 z)
{
    return cx_cube(z) - 1.0;
}

vec2 cx_inverse(vec2 z)
{
    float divider = z.x * z.x + z.y * z.y;
    return vec2(z.x, -z.y) / divider;
}

void main()
{
    vec2 c = vec2(vertexColor.z, vertexColor.w);
    vec2 z = vec2(vertexColor.x, vertexColor.y);

    for (float i = 0; i < iterationsCount; ++i) {
        z = z - (cx_inverse(cx_sqr(vec2(z.y, -z.x))) + z) / 3.0;

        vec2 t = f(z);

        float l1 = length(t - vec2(-0.5, sqrt(3.0) / 2.0));
        float l2 = length(t - vec2(-0.5, -sqrt(3.0) / 2.0));

        if (length(l1) < 0.3 || length(l2) < 0.3) {
            FragColor = vec4(newtonGetColor(i / iterationsCount), 1.0);
            return;
        }
    }

    FragColor = vec4(0.3, 0.3, 0.3, 0.1);
}