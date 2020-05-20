#version 150 core

in vec4 ourPosition;
in vec3 ourNormal;

out vec4 outColor;


// -----------------------
// Oswietlenie
struct DLightParameters
{
    vec4 Direction;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
};
uniform DLightParameters secondLight;


struct Material
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};
uniform Material secondMaterial;

void main()
{
	// ambient
    vec3 ambientPart = vec3(secondLight.Ambient) * secondMaterial.Diffuse;
  	
    // diffuse 
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(-secondLight.Direction);  
    float resultDiffuse = max(dot(norm, secondLight.Direction), 0.0);
    vec3 diffusePart = secondLight.Diffuse * resultDiffuse * secondMaterial.Diffuse;
    
    // specular
    vec3 viewDir = normalize(secondLight.Position - ourPosition);
    vec3 reflectDir = reflect(-secondLight.Direction, ourNormal);  
    float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), secondMaterial.Shininess);
    vec3 specularPart = specularCoeff * vec3(secondLight.Specular) * secondMaterial.Specular;
        
    return (ambientPart + diffusePart + specularPart);
}
