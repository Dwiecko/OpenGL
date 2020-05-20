#version 150

in vec4 inoutPos;
in vec2 inoutUV;

out vec4 outColor;

uniform sampler2D tex0;
uniform float hasCollision;

void main()
{
	vec2 newUV = vec2(inoutUV.s, 1.0 - inoutUV.t);
	outColor = texture( tex0, newUV );

	if(hasCollision == 1.0f)
	{
		outColor -= vec4(1.0f, 0.0f, 0.0f, 0.0f);
	}
}
