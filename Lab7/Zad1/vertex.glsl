#version 330


layout( location = 0 ) in vec4 inPosition;
//layout( location = 1 ) in vec4 inColor;

// Przekazanie zmiennych do kolejnego etapu

//out vec4 inoutColor;
//out vec4 inoutCoord;

// struktura danych wyjsciowych
out VS_OUT
{
	vec4 coord;
//	vec4 color;

} vs_out;

uniform mat4 Matrix_proj_mv;


void main()
{
	gl_Position = Matrix_proj_mv * inPosition;

	vs_out.coord = inPosition;
}
