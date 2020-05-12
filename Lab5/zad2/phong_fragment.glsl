#version 150 core

in vec4 ourPosition;
in vec3 ourNormal;

out vec4 outColor;


// Parametry oswietlenia
uniform vec3 Light_Ambient;
uniform vec3 Light_Diffuse;
uniform vec3 Light_Position;


void main()
{

	// ustawienie domyslnego koloru fragmentu
	// mozna tutaj uwzglednic tekstury i inne parametry
	vec3 objectColor = vec3(0.2, 0.8, 0.2);

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek

	vec3 lightDirection = normalize(Light_Position - vec3(ourPosition));

	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float lightCoeff = max(dot(ourNormal, lightDirection), 0.0);

	// obliczenie skladowej diffuse
	vec3 resultDiffuse = lightCoeff * Light_Diffuse;

	// Zastosowanie oswietlenia do fragmentu
	vec3 result = (Light_Ambient + resultDiffuse) * objectColor;

	outColor = vec4(result, 1.0);

}
