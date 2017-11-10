#version 330 core

in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec2 texCoord;

uniform mat4 mvp;
uniform sampler2D heightTexture;

void main()
{
    // Flipped texture coordinates
    texCoord = 1.0 - vertexTexCoord;

    // Calculate y value based upon input coordinates and time
    vec3 pos = vertexPosition;

    pos.y = 1.5 * length(texture(heightTexture, texCoord).rgb);

    gl_Position = mvp * vec4( pos, 1.0 );
}
