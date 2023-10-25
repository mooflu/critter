uniform sampler2D textureUnit;
uniform int withTexture;
uniform vec4 aColor;

in vec2 v_uv;
in vec4 v_color;

out vec4 fragColor;

void main()
{
    vec4 theColor = aColor;
    if (theColor.r < 0.0) {
        theColor = v_color;
    }

    if (withTexture == 1) {
        fragColor = texture( textureUnit, v_uv ) * theColor;
    } else {
        fragColor = theColor;
    }
}
