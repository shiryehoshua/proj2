#version 150

// Sample vertex shader for Project 2.  Hack away!

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

void main() {
  // transform vertices 
  gl_Position = projMatrix * viewMatrix * modelMatrix * vertPos;

  // transform normals
  vec3 nrm = normalize(normalMatrix * vertNorm);

  // max(0, n dot l)
  float ndotl = max(0, dot(nrm, lightDir));// < 0 ? 0 : dot(nrm, lightDir);

  vec3 diff, amb, spec;
  diff.r = diff.g = diff.b = 0;
  spec.r = spec.g = spec.b = 0;

  amb  = Ka * objColor;
  diff = Kd * objColor * lightColor * dot(nrm, lightDir); 

  // Set the color
  fragColor.rgb = amb + diff + spec;

  // Surface brightest when normal and lightDir are aligned, but this
  // is not Phong lighting, and may not be a good starting point for it.
  float dt = pow((dot(nrm, lightDir)+1.0)/2.0, 4);
  //fragColor.rgb = vertRgb * objColor.rgb * (Ka + Kd*dt);
  fragColor.a = 1.0;
}
