#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;

varying vec2 texCoord;
varying vec3 color;

const vec3 lightSource = vec3(1, 1, -1);

uniform mat4 mvp;

void main()
{
    // Flipped texture coordinates
    texCoord = vertexTexCoord;

    float theta = 1.5 * dot(normalize(lightSource), vertexNormal);

    color = vec3(0.5 * (cos(theta)+1),
                         0.5 * (cos(theta)+1),
                         0.5 * ((1.0 * cos(theta)) + 1));

    gl_Position = mvp * vec4(vertexPosition, 1.0 );
}
