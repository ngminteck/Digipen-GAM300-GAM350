#version 420

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 uvs;
layout (location = 4) in vec3 tan;
layout (location = 5) in vec3 biTan;

uniform mat4 vpMat;
uniform mat4 mMat;
uniform mat4 sMat;

void main()
{
  mat4 MS = mMat * sMat;
  gl_Position = vpMat * MS * vec4(pos, 1.0);
}