#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;

varying vec2 texCoord;

uniform mat4 mvp;

void main()
{
    // Flipped texture coordinates
    texCoord = vertexTexCoord;
    texCoord.x = 1.0 - texCoord.x;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
