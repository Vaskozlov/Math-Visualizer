#version 330 core

in vec2 TexCoord;
uniform sampler3D texture1;
uniform vec3 fixedLevel;
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

    float texColor = texture(texture1, textureCoord).r;
    FragColor = vec4(floatToColor(texColor, 0.0, 1.0), 1.0);
}

