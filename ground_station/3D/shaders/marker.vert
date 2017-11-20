#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;

varying vec2 texCoord;

uniform mat4 mvp;

void main()
{
    texCoord = vertexTexCoord;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
