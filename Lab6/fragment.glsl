#version 150 core

in vec4 ourPosition;
in vec3 ourNormal;

out vec4 outColor;

uniform vec3 Camera_Position;


// -----------------------
// Oswietlenie
struct LightParameters
{
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Position;
    float Attenuation;
};
uniform LightParameters myLight;


// -----------------------
// Materialy
struct Material
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};
uniform Material myMaterial;



// ---------------------------------------------------------------------------
vec3 Calculate_PointLight(vec3 viewDir)
{

	float Light_dist = length(myLight.Position - ourPosition);
	float Light_dist_coeff = exp(-Light_dist/20.0f);
	float attenuation = 1.0 / (1.0 + myLight.Attenuation * (Light_dist * Light_dist));


	// ------------------
	// Skladowa: Ambient
	vec3 ambientPart = vec3(myLight.Ambient) * myMaterial.Ambient * attenuation;

	// ------------------
	// Skladowa: Diffuse

	// Obliczenie wektora (swiatlo - wierzcholek)
	// czyli kierunku padania swiatla na wierzcholek
	vec3 lightDirection = vec3(normalize(myLight.Position - ourPosition));


	// obliczenie kata pomiedzy wektorem lightDir oraz wektorem normalnym
	// wartosc kata okresla pod jakim katem padaja promienie
	float diffuseCoeff = max(dot(ourNormal, lightDirection), 0.0);

	// obliczenie skladowej diffuse
	vec3 resultDiffuse = diffuseCoeff * vec3(myLight.Diffuse) * myMaterial.Diffuse;

	vec3 diffusePart = resultDiffuse * attenuation;


	// ------------------
	// Specular
	vec3  reflectDir = reflect(-lightDirection, ourNormal);
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), myMaterial.Shininess);
	vec3  resultSpecular = specularCoeff * vec3(myLight.Specular) * myMaterial.Specular;

	vec3 specularPart = resultSpecular * attenuation;

	return (ambientPart + diffusePart + specularPart);
}



void main()
{

	// Kolor startowy
	vec3 objectColor = vec3(abs(sin(length(ourPosition))), 1.0, 1.0);

	vec3 viewDir = normalize(Camera_Position - vec3(ourPosition));

	// Obliczanie swiatel punktowych
	vec3 lightPart = vec3(0.0);

	lightPart += Calculate_PointLight(viewDir) * objectColor;

	outColor = vec4(lightPart, 1.0);


}
