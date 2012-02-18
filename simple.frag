#version 150

// Sample fragment shader for Project 2.  Hack away!

uniform int gouraudMode;
uniform int gi;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform sampler2D samplerA;
uniform sampler2D samplerB;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 fragColor;
in vec2 texCoord;
in vec3 vnrm;

out vec4 color;

void main() {
  color = fragColor;
}
