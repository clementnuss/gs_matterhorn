#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec2 texCoord;

uniform mat4 mvp;

void main()
{
    // Calculate y value based upon input coordinates and time
    vec3 pos = vertexPosition;

    // Pass through texture coordinates
    texCoord = vertexTexCoord;

    gl_Position = mvp * vec4( pos, 1.0 );
}
