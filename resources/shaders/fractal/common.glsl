#version 330

vec2 cx_sqr(vec2 value)
{
    return vec2(value.x * value.x - value.y * value.y, 2.0 * value.x * value.y);
}

vec2 cx_cube(vec2 z)
{
    return vec2(z.x * z.x * z.x - 3.0 * z.x * z.y * z.y, 3 * z.x * z.x * z.y - z.y * z.y * z.y);
}

vec2 cx_inverse(vec2 z)
{
    float divider = z.x * z.x + z.y * z.y;
    return vec2(z.x, -z.y) / divider;
}

vec2 cx_div(vec2 a, vec2 b)
{
    float divider = b.x * b.x + b.y * b.y;
    return vec2(a.x * b.x + a.y * b.y, a.y * b.x - a.x * b.y) / divider;
}

vec2 mandelbrotF(vec2 value, vec2 c)
{
    return cx_sqr(value) + c;
}

vec2 newtonClassicFunction(vec2 z)
{
    return cx_cube(z) - 1.0;
}

vec2 cx_sin(vec2 z)
{
    return vec2(sin(z.x) * cosh(z.y), cos(z.x) * sinh(z.y));
}

vec2 cx_cos(vec2 z)
{
    return vec2(cos(z.x) * cosh(z.y), -sin(z.x) * sinh(z.y));
}

vec3 mandelbrotGetColor(float k)
{
    if (k < 0.33) {
        return vec3(0.6, 0.5, 0.3) * (k / 0.25);
    }

    if (k < 0.66) {
        return vec3(0.3, 0.0, 0.6) * (k / 0.33 - 1.0);
    }

    return vec3(0.05, 0.05, 0.3) * (k / 0.33 - 2.0);
}

vec3 getColorNoScale(float k)
{
    if (k < 0.33) {
        return vec3(0.6, 0.5, 0.3);
    }

    if (k < 0.66) {
        return vec3(0.3, 0.0, 0.6);
    }

    return vec3(0.05, 0.05, 0.3);
}

vec3 getColorExpScale(float k)
{
    if (k < 0.33) {
        return vec3(0.6, 0.5, 0.3) * exp(k);
    }

    if (k < 0.66) {
        return vec3(0.3, 0.0, 0.6) * exp(k);
    }

    return vec3(0.05, 0.05, 0.3) * exp(k);
}
