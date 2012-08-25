attribute vec4 vPosition;
attribute vec2 vTexCoords;

varying vec2 vVertTexCoords;

void main()
{
    gl_Position = vPosition;
    vVertTexCoords = vTexCoords;
}