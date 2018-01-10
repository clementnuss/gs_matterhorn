#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;

varying vec2 texCoord;
varying float intensity;

uniform mat4 mvp;
uniform sampler2D heightTexture;

void main()
{
    // Flipped texture coordinates
    texCoord = vertexTexCoord;

    // Calculate y value based upon input coordinates and time
    vec3 pos = vertexPosition;

    intensity = vertexNormal.y;

    //pos.y = 260 * length(texture2D(heightTexture, texCoord).rgb);

    gl_Position = mvp * vec4( pos, 1.0 );
}
