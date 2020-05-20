#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec4 ourPosition;
out vec3 ourNormal;

uniform mat4 matProjection;
uniform mat4 matView;

void main()
{

	gl_Position = matProjection * matView * inPosition;
	ourPosition = inPosition;
	ourNormal = inNormal;
}

