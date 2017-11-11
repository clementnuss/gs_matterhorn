#version 330 core

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main()
{
    fragColor = texture(diffuseTexture, texCoord).rgba;
}
