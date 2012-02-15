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
  vec4 v = normalize(viewMatrix * vertPos);
  vec3 view; view.x = v.x; view.y = v.y; view.z = v.z;
  vec3 h = normalize(lightDir + view); 
  float ndotl = max(0, dot(nrm, lightDir));
  float ndoth = max(0, dot(nrm, h));

  vec3 diff, amb, spec;
  amb  = Ka * objColor;
  diff = Kd * objColor * lightColor * ndotl; 
  spec = Ks * lightColor * ndoth;

  // Set the color
  fragColor.rgb = amb + diff + spec;

  // Surface brightest when normal and lightDir are aligned, but this
  // is not Phong lighting, and may not be a good starting point for it.
  float dt = pow((dot(nrm, lightDir)+1.0)/2.0, 4);
  //fragColor.rgb = vertRgb * objColor.rgb * (Ka + Kd*dt);
  fragColor.a = 1.0;
}