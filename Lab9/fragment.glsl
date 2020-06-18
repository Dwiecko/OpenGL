#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;


void main()
{

	vec2 newUV = vec2(inoutUV.s, 1.0 - inoutUV.t);
	outColor = texture( tex0, newUV );// + vec4(myColor, 1.0)/2.0;

}
