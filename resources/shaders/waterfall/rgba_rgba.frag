#version 330 core

in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
out vec4 FragColor;

void main()
{
    vec4 firstTexColor = texture(texture1, TexCoord);
    vec4 secondTexColor = texture(texture2, TexCoord);
    vec4 texColor = mix(firstTexColor, secondTexColor, secondTexColor.a);

    if (texColor.a < 1e-8)
        discard;

    FragColor = texColor;
}

