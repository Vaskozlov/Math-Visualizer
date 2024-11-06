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

vec2 f(vec2 value, vec2 c)
{
    return squareComplex(value) + c;
}

void main()
{
    vec2 c = vec2(vertexColor.z, vertexColor.w);
    vec2 z = vec2(vertexColor.x, vertexColor.y);
    vec3 t = vec3(0.0);
    float i = 0;
    float max_i = iterationsCount;

    for (; i < max_i; ++i) {
        z = f(z, c);

        if (length(z) > 2.0) {
            FragColor = vec4(getColor(i / max_i), 1.0);
            return;
        }
    }

    FragColor = vec4(0.05, 0.05, 0.3, 1.0);
}