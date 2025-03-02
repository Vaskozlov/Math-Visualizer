#version 330 core

in vec2 TexCoord;
in vec2 valueLimits;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 FragColor;

vec3 floatToColor(float value, float min_value, float max_value);

void main()
{
    float value = texture(texture1, TexCoord).r;
    vec4 mask = texture(texture2, TexCoord);

    vec4 hsv_color = vec4(floatToColor(value, valueLimits.x, valueLimits.y), 255);
    vec4 texColor = mix(hsv_color, mask, mask.a);

    if (texColor.a < 1e-8){
        discard;
    }

    FragColor = texColor;
}

