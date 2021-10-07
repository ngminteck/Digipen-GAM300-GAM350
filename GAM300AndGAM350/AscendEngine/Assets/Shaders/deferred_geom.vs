#version 420

/*  This is required for GLSL below 4.3 to explicitly define uniform variable locations */
#extension GL_ARB_explicit_uniform_location : require

/*  Vertex data */
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 uv;
layout (location = 4) in vec3 tan;
layout (location = 5) in vec3 biTan;

uniform mat4 mMat;
uniform mat4 vMat;
uniform mat4 pMat;
uniform mat4 nrmMat;
uniform int isDebug;

uniform vec2 uvOffset;
uniform vec2 tile;

/*  Define outputs of vertex shader: pos, nrm and uv */
out vec4 posVF;
out vec4 posWF;
out vec4 nrmVF;
out vec4 nrmWF;
out vec2 uvCoord;
out vec4 posCS;
//out float depth;


/*	Compute gl_Position, uv, and nrm, pos in appropriate frames  */
void main(void)
{
  mat4 mvMat = vMat * mMat;

  vec2 u = vec2(uv.x, uv.y);
  uvCoord = (u * tile) + uvOffset;

  nrmVF = transpose(inverse(mvMat)) * vec4(nrm,1.0);
  nrmWF = transpose(inverse(mMat)) * vec4(nrm,1.0);

  posVF = mvMat * vec4(pos,1.0);
  posWF = mMat * vec4(pos, 1.0);

  gl_Position = posCS = pMat * posVF;
}