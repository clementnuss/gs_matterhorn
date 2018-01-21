#version 120

varying vec2 texCoord;
varying vec3 color;

uniform sampler2D diffuseTexture;

void main()
{
    vec3 texColor = texture2D(diffuseTexture, texCoord).rgb;
    gl_FragColor = vec4(color.x * texColor.x, color.y * texColor.y, color.z * texColor.z, 1.0);
}
