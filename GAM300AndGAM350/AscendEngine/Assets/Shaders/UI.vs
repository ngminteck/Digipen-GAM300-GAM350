#version 420

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 uv;
layout (location = 4) in vec3 tan;
layout (location = 5) in vec3 biTan;

uniform vec2 uvOffset;
uniform vec2 tile;

uniform mat4 finalMat;
//uniform mat4 sMat;
//uniform mat4 mMat;

out vec2 uvCoord;

void main()
{
  vec2 u = vec2(uv.x, 1 - uv.y);
  uvCoord = (u * tile) + uvOffset;
  gl_Position = finalMat * vec4(pos, 1.0);

  //gl_Position = sMat * mMat * vec4(pos, 1.0);
}