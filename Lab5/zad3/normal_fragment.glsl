#version 150 core

out vec4 outColor;

in vec3 ourNormal;

void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	//
	vec3 result = ourNormal;

	outColor = vec4(result, 1.0);

}
