#version 150

in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

out vec4 outColor;

// Sampler tekstury
uniform sampler2D tex0;
uniform sampler2D tex1;


void main()
{

	outColor = mix(texture(tex0, inoutUV), texture(tex1, inoutUV), 0.3);
}
