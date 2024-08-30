#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// Tile uniform values
uniform vec2 tilePosition;
uniform vec2 tileSize;

void main()
{
    vec2 modded = vec2(mod(fragTexCoord.x, tileSize.x), mod(fragTexCoord.y, tileSize.y));
    vec2 offset = tilePosition + modded;

    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, offset);

    // NOTE: Implement here your fragment shader code

    finalColor = texelColor*colDiffuse;
}
