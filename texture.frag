#version 150 

// Sample fragment shader for Project 2.  Hack away!

uniform int gouraudMode;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform sampler2D sampler; 
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 fragColor;
in vec2 texCoord;
in vec3 vnrm;

out vec4 color;

void main() {

  vec4 textureColor = texture(sampler, texCoord);

  if (gouraudMode != 0) { // in Gouraud mode
    color = fragColor;
  }
  else { // in Phong mode
    vec3 diff = Kd * max(0.0, dot(vnrm, lightDir)) * objColor;
    vec3 amb = Ka * textureColor.rgb;

    vec3 r = normalize(reflect(-normalize(lightDir), normalize(vnrm)));
    float vnrmdotr = max(0.0, dot(normalize(vnrm), r));
    vec3 spec = Ks * pow(vnrmdotr, shexp) * lightColor;

    color.rgb = diff + amb + spec;
    color.a = 1.0;
  }
}
