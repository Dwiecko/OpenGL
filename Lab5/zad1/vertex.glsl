#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;

uniform mat4 Matrix_proj_mv;

out vec4 inoutPos;
out vec3 inoutColor;
out vec2 inoutUV;


void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

	inoutPos = inPosition;
	inoutColor = inColor;
	inoutUV = inUV;
}
