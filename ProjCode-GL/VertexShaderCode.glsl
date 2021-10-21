#version 450

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 modelTransformMatrix;
uniform mat4 viewTransformMatrix;
uniform mat4 projectionMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 out_position = projectionMatrix * viewTransformMatrix * modelTransformMatrix * v;
	gl_Position = out_position;	
	theColor = vertexColor;
}
