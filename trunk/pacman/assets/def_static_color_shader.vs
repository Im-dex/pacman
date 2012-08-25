attribute vec4 vPosition;
attribute vec4 vColor;

varying vec4 vVertColor;

void main() 
{
    vVertColor = vColor;
    gl_Position = vPosition;
}