#version 450

in layout(location = 0) vec3 position;
out vec3 TexCoords;

uniform mat4 viewTransformMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * viewTransformMatrix * vec4(position, 1.0f);
	TexCoords = position;
}
