#version 120

varying vec2 texCoord;
varying float intensity;

uniform sampler2D diffuseTexture;

void main()
{
    //gl_FragColor = vec4(texture2D(diffuseTexture, texCoord).rgb, 1.0);

    gl_FragColor = vec4(vec3(intensity), 1.0);
}
