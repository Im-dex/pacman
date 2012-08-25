attribute vec4 vPosition;
attribute vec2 vTexCoords;

uniform mat4 mModelProjectionMatrix;
varying vec2 vVertTexCoords;

void main()
{
	gl_Position = mModelProjectionMatrix * vPosition;
	vVertTexCoords = vTexCoords;
}