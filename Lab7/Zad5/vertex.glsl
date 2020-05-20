#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;


uniform mat4 Matrix_proj_mv;
uniform mat4 matModel = mat4x4(1.0);

out vec4 inoutPos;
out vec2 inoutUV;

void main()
{
	gl_Position = Matrix_proj_mv * matModel * inPosition;

	inoutPos = inPosition;
	inoutUV = inUV;
}
