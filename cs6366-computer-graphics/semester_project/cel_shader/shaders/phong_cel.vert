#version 130

uniform mat4 modelViewMatr;
uniform mat4 projMatr;
uniform bool lightsEnabled;

in vec4 globalAmbient;
in vec4 lightAmbient;
in vec4 lightDiffuse;
in vec4 lightSpecular;

in vec4 matAmbient;
in vec4 matDiffuse;
in vec4 matSpecular;
in float matShininess;

in vec3 lightLocation;
in vec3 viewerLocation;
in vec3 vertexNormal;
in vec3 vertexPosition;

smooth out vec4 vColor;
smooth out float intensity;

void main() {
  // transform vertex position and normal into eye coords
  vec3 vertPos = (modelViewMatr * vec4(vertexPosition, 1.0)).xyz;
  vec3 N = normalize(modelViewMatr * vec4(vertexNormal, 0.0)).xyz; 

  // Light to vertex vector
  vec3 L = normalize(-vertPos);                                    

  intensity = dot( L, N );

  vec4 localDiffuseProduct = (lightDiffuse * matDiffuse); 

  vColor = localDiffuseProduct;

  gl_Position = projMatr * modelViewMatr * vec4(vertexPosition, 1.0);
}