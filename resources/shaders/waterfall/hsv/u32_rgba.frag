#version 330 core

in vec2 TexCoord;
in uvec2 valueLimits;
uniform usampler2D texture1;
uniform sampler2D texture2;
out vec4 FragColor;

vec3 hsvUintToRgb(uint value, uint min_value, uint max_value);

void main()
{
    uint value = texture(texture1, TexCoord).r;
    vec4 mask = texture(texture2, TexCoord);

    vec4 hsv_color = vec4(hsvUintToRgb(value, valueLimits.x, valueLimits.y), 1.0);
    vec4 texColor = mix(hsv_color, mask, mask.a);

    if (texColor.a < 1e-8){
        discard;
    }

    FragColor = texColor;
}

