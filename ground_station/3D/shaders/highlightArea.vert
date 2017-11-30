#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform sampler2D heightTexture;

void main()
{
    // Flipped texture coordinates
    vec2 texCoord = (modelMatrix * vec4(vertexPosition,1.0)).xz;
    texCoord += vec2(5000,5000);
    texCoord /= 10000;
    texCoord.x = 1.0 - texCoord.x;

    //texCoord = texCoord / 2.0;

    // Calculate y value based upon input coordinates and time
    vec3 pos = vertexPosition;

    pos.y = 260 * length(texture2D(heightTexture, texCoord).rgb);

    gl_Position = mvp * vec4( pos, 1.0 );
}
