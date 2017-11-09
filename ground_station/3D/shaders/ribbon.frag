#version 330 core

out vec4 fragColor;

uniform vec3 ka;            // Ambient reflectivity
uniform vec3 kd;            // Diffuse reflectivity

uniform sampler2D diffuseTexture;

void main()
{
    fragColor = vec4(texture(diffuseTexture, texCoord).rgb , 1.0);
}
