#version 420

in vec2 uvCoord;

uniform vec4 tint;
uniform sampler2D tex_diffuse;

out vec4 outColor;

uniform float gamma;
void main()
{
float temp =0.0f;
  vec4  diff = texture( tex_diffuse, uvCoord );
  if(gamma <= 0.8)
  temp = gamma;
  else
    temp = gamma- 0.8f;
  

  diff.rgb = pow(diff.rgb, vec3(1.0f/temp));
 
    // Gamma correction 
  outColor = diff * tint;
}