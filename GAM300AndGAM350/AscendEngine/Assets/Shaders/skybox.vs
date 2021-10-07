#version 420 core

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float aspRatio;

out vec3 texCoord;

const vec2 quad[4] = vec2[4]( 
  vec2( -1.0, -1.0), 
  vec2(  1.0, -1.0), 
  vec2( -1.0,  1.0), 
  vec2(  1.0,  1.0)
);

vec3 corners[4];

void main(void)
{
  //@TODO
  /*  Render background using a full-screen quad defined as the far plane in NDC frame.

    This quad texture coordinates are defined in world-frame as the 4 back corners 
    of a 2x2x2 box symmetric about the world origin.
  */
  gl_Position = vec4(quad[gl_VertexID], 1.0, 1.0);
  
  mat4 invProj = inverse(projMat);
  mat3 transView = transpose(mat3(viewMat));
  vec3 wldpos = -transView[2];

  //vec3 xView = vec3(transView[0]) * aspRatio;
  //vec3 yView = vec3(transView[1]);

  //corners[0] = wldpos - xView - yView;
  //corners[1] = wldpos + xView - yView;
  //corners[2] = wldpos - xView + yView;
  //corners[3] = wldpos + xView + yView;

  //texCoord = corners[gl_VertexID];

  //zelin implementation
  vec3 dir = vec3(quad[gl_VertexID], -1.0);
  texCoord = transView * (invProj * gl_Position).xyz;
}