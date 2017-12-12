#version 120

varying vec2 texCoord;

uniform sampler2D diffuseTexture;

void main()
{
    gl_FragColor = vec4(texture2D(diffuseTexture, texCoord).rgb, 1.0);
}
