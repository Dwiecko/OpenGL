#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
	outColor = texture( tex0, inoutUV );// + vec4(vec3(211.0f, 211.0f, 211.0f), 1.0)/2.0;
}
