/** This represents the texture used to determine visible parts. */
uniform sampler2D maskTexture;

/** This represents the color of the visible parts. */
uniform vec3 visibleColor;

/** This represents the color of the invisible parts. */
uniform vec3 invisibleColor;

/**
 * This is a visibility mask fragment shader, it is meant to be
 * used for creating 2D visibility masks. The following steps
 * will help you do this:
 *
 *   - Create an sf::RenderTexture the size of the relevant window
 *   - Draw whatever you want to that texture
 *   - Create an sf::Sprite using that texture
 *   - Draw that sprite to the window using this shader
 *
 * Any fragment on your generated texture that has a maximum red
 * value will appear entirely transparent and in the specified
 * 'visible' color, while fragments with a minimum red value will
 * appear entirely opaque and in the color of the specified mask color.
 */
void main()
{
    // Get the color of the mask at this pixel.
    vec4 maskp = clamp(texture2D(maskTexture, gl_TexCoord[0].xy), 0.0, 1.0);
    float alpha = 1.0 - maskp.r;
    vec3 color = mix(visibleColor, invisibleColor, alpha);
    gl_FragColor = vec4(color, alpha);
}
