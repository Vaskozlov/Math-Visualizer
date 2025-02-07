#version 330 core

in vec2 TexCoord;
uniform sampler2D texture1;
out vec4 FragColor;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);

    if(texColor.a < 1e-8)
        discard;

    FragColor = texColor;
}

