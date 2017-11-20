#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;

varying vec2 texCoord;

uniform mat4 mvp;
uniform sampler2D heightTexture;

void main()
{
    // Flipped texture coordinates
    texCoord = vertexTexCoord;
    texCoord.x = 1.0 - texCoord.x;

    // Calculate y value based upon input coordinates and time
    vec3 pos = vertexPosition;

    pos.y = 5 * length(texture2D(heightTexture, texCoord).rgb);

    gl_Position = mvp * vec4( pos, 1.0 );
}
