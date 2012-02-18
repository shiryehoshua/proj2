#version 150 core

// Sample fragment shader for Project 2.  Hack away!

/*in vec4 fragColor;
out vec4 color;*/

uniform int gouraudMode;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 fragColor;
in vec3 vnrm;

out vec4 color;

void main() {
  color = fragColor;
}
