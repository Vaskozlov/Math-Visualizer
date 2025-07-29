in vec2 TexCoord;
in vec2 valueLimits;
uniform sampler2D texture1;
out vec4 FragColor;

vec3 floatToColor(float value, float min_value, float max_value);

void main()
{
    float value = texture(texture1, TexCoord).r;

    if (isnan(value)) {
        FragColor = vec4(0);
        return;
    }

    if (isinf(value)) {
        FragColor = vec4(1.0);
        return;
    }

    FragColor = vec4(floatToColor(value, valueLimits.x, valueLimits.y), 1.0);
}
