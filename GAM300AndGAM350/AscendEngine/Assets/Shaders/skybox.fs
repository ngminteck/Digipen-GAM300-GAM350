#version 420 core

uniform samplerCube tex_Cube;

in vec3 texCoord;

out vec4 fragColor;

void main(void)
{
  fragColor = texture(tex_Cube, texCoord);
}