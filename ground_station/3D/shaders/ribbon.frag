#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main()
{
    fragColor = vec4(texture(diffuseTexture, texCoord).rgb, 1.0);
}
