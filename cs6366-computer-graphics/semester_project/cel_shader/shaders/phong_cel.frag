#version 130

uniform int numLevels;

smooth in vec4 vColor;
smooth in float intensity;

out vec4 fColor;

vec4 toonify( in vec4 color, in float brightness, in float levels );

void main() {
  fColor = toonify( vColor, intensity, numLevels );
}  

vec4 toonify( in vec4 color, in float brightness, in float levels ) {
  vec4 res = ceil(brightness * levels) / levels * color;
  res.a = color.a;
  return res;
}