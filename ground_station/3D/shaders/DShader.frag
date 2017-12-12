#version 120

varying vec2 texCoord;

uniform sampler2D diffuseTexture;

void main()
{
    gl_FragColor = texture2D(diffuseTexture, texCoord).rgba;
}
