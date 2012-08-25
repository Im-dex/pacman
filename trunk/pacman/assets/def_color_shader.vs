attribute vec4 vPosition;
attribute vec4 vColor;

uniform mat4 mModelProjectionMatrix;

varying vec4 vVertColor;

void main() 
{
    vVertColor = vColor;
    gl_Position = mModelProjectionMatrix * vPosition;
}
