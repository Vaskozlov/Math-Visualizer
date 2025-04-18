#version 330 core

in vec2 TexCoord;
in vec2 valueLimits;
in vec4 imageMaxSize;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 FragColor;

vec3 hsvFloatToRgb(float value, float min_value, float max_value);

void main()
{
    if (gl_FragCoord.x < imageMaxSize.x || gl_FragCoord.x > imageMaxSize.z || gl_FragCoord.y < imageMaxSize.y || gl_FragCoord.y > imageMaxSize.w){
        discard;
    }

    float value = texture(texture1, TexCoord).r;

    if (isnan(value)) {
        FragColor = vec4(0);
        return;
    }

    vec4 mask = texture(texture2, TexCoord);

    vec4 hsv_color = vec4(hsvFloatToRgb(value, valueLimits.x, valueLimits.y), 255);
    vec4 texColor = mix(hsv_color, mask, mask.a);

    if (texColor.a < 1e-8){
        discard;
    }

    FragColor = texColor;
}

