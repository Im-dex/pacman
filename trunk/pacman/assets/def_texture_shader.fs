precision mediump float;

uniform sampler2D colorTexture;

varying vec2 vVertTexCoords;

void main()
{
    gl_FragColor = texture2D(colorTexture, vVertTexCoords);
}