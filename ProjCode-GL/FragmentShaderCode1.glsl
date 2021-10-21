#version 450

in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{
	color = vec4(texture(skybox, TexCoords));
}