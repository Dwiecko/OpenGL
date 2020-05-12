#version 150

//in vec4 inoutColor;
//in vec4 inoutCoord;

in VS_OUT
{
	vec4 coord;
//	vec4 color;
} fs_in;

out vec4 outColor;



void main()
{

	// szalone kolory
    float odl = sqrt(fs_in.coord.x*fs_in.coord.x + fs_in.coord.y*fs_in.coord.y + fs_in.coord.z*fs_in.coord.z)/5.0f;
	float x = sin(odl*13.0f)/2.0f + 0.5f;
	float y = sin(fs_in.coord.y)/2.0f + 0.5f;
	float z = sin(gl_PrimitiveID/100.0f)/5.0f + 0.5f;
	float a = 1.0f;
	outColor = vec4(x, y, z, a);

	outColor = vec4(1.0f, fs_in.coord.y, 0.0f, 1.0f);
}
