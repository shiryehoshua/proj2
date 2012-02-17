#version 150

// Sample vertex shader for Project 2.  Hack away!

uniform int gouraudMode;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 lightDir;  // assumed to be unit-length (already normalized)
uniform vec3 lightColor;
uniform vec3 objColor;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 vertPos;
in vec3 vertRgb;
in vec3 vertNorm;

out vec4 fragColor;
out vec3 vnrm;

void main() {

  // transform vertices 
  gl_Position = projMatrix * viewMatrix * modelMatrix * vertPos;
  
  // surface normal in view coords
  vnrm = normalMatrix * vertNorm;

  fragColor.rgb = objColor;
  fragColor.a = 1.0;
}
