#version 420

uniform vec3 tint;

out vec4 outColor;

void main()
{
  outColor = vec4(tint, 1.0);
}