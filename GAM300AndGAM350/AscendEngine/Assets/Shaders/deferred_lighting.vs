#version 430

/*  This is required for GLSL below 4.3 to explicitly define uniform variable locations */
#extension GL_ARB_explicit_uniform_location : require

/*  Vertex data */
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;

out vec2 uvCoord;

void main(void)
{
  uvCoord = uv.xy;
  gl_Position = vec4(pos, 0.0, 1.0);
}