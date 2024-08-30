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

void main()
{
    vec2 tileSize = vec2(1.0, 0.25);

    vec2 modded = tilePosition + mod(fragTexCoord, tileSize);

    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, modded);

    // NOTE: Implement here your fragment shader code

    finalColor = fragColor*texelColor;
    //finalColor = vec4(1.0, 0.0, 1.0, 1.0);
}
