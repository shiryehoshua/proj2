#version 150 

// Fragment shader for texture mapping 

#define PI_INV 0.31830988618379067153776752674 

uniform int gouraudMode;
uniform int seamFix;
uniform int gi;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform sampler2D samplerA;
uniform sampler2D samplerB;
uniform sampler2D samplerC;
uniform sampler2D samplerD;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shexp;

in vec4 fragColor;
in vec3 texCoord;
in vec3 vnrm;

out vec4 color;

void main() {

  vec4 c;
  vec2 tc;
  // seamless
  if (seamFix!=0) { 
    // recover theta from the vertex shader
    // atan(sin(theta)/cos(theta))
    tc.x = -0.5 * PI_INV * atan(texCoord.z, texCoord.x) ;
    tc.y = texCoord.y;
  } else {
    tc = texCoord.xy;
  }

  if (gi==0) { // sphere
    c = texture(samplerA, tc);
  } else { // square
    c = texture(samplerD, tc*100);
  }

  if (gi==0) {
    // Phong
    vec3 diff = Kd * max(0.0, dot(vnrm, lightDir)) * objColor;
    vec3 amb = Ka * c.rgb;

    vec3 r = normalize(reflect(-normalize(lightDir), normalize(vnrm)));
    float vnrmdotr = max(0.0, dot(normalize(vnrm), r));
    vec3 spec = Ks * pow(vnrmdotr, shexp) * lightColor;

    color.rgb = diff + amb + spec;
    color.a = 1.0;
  } else {
    vec3 amb = Ka * c.rgb;
    color.rgb = amb;
    color.a = 1.0;
  }
}
