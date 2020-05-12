#version 330


layout( location = 0 ) in vec4 inPosition;
out VS_OUT
{
	vec4 coord;
} vs_out;

uniform mat4 Matrix_proj_mv;


void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

	vs_out.coord = inPosition;
}
