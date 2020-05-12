#version 150 core

out vec4 outColor;


void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);


	outColor = vec4(objectColor, 1.0);

}
