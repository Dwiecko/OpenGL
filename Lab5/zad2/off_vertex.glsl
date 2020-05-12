#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;

uniform mat4 Matrix_proj_mv;



void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

}
