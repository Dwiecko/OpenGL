#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

out vec4 ourPosition;
out vec3 ourNormal;

uniform mat4 Matrix_proj_mv;

void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

	ourPosition = inPosition;
	ourNormal = inNormal;
}
