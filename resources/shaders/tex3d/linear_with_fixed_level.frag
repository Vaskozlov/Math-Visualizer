#version 330 core

in vec2 TexCoord;
uniform sampler3D texture1;
uniform vec3 fixedLevel;
uniform vec2 valueRange;
uniform float fixedLevelValue;

out vec4 FragColor;

vec3 floatToColor(float value, float min_value, float max_value);

void main()
{
    vec3 textureCoord;

    if (isnan(fixedLevel.x)) {
        textureCoord = vec3(fixedLevelValue, TexCoord.x, TexCoord.y);
    } else if (isnan(fixedLevel.y)) {
        textureCoord = vec3(TexCoord.x, fixedLevelValue, TexCoord.y);
    } else {
        textureCoord = vec3(TexCoord, fixedLevelValue);
    }

    float value = texture(texture1, textureCoord).r;

    if (valueRange.x > value || valueRange.y < value) {
        discard;
    }

    FragColor = vec4(floatToColor(value, valueRange.x, valueRange.y), 1.0);
}

