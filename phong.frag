#version 150 core

// Sample fragment shader for Project 2.  Hack away!

uniform int gouraudMode;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 objColor;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 fragColor;

out vec4 color;

void main() {

  if (true) {
    color = fragColor;
    color.a = 1.0;
  }
  else { // in Phong mode
    // ambient value stays the same

    // diffuse is calculated differently
   // diff = Kd * objColor * max(0, dot(normalize(nrm), normalize(vlightDir)));

    // specular uses different reflection
    //spec = Ks * lightColor * pow(ndotr, shexp);
  }
}
