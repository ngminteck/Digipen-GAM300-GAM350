#version 420

/*  This is required for GLSL below 4.3 to explicitly define uniform variable locations */
#extension GL_ARB_explicit_uniform_location : require
precision highp float;

in vec4 posVF;
in vec4 posWF;
in vec4 nrmVF;
in vec4 nrmWF;
in vec2 uvCoord;
in vec4 posCS;

uniform sampler2D tex_diffuse;
uniform sampler2D tex_ao;
uniform sampler2D tex_gloss;
uniform sampler2D tex_normal;
uniform vec4 tint;
uniform float roughness;
uniform float metalness;

uniform int showMetric;
uniform sampler2D tex_metric;

uniform vec3 pickCol;

layout(location = 0) out vec3  gPosition;
layout(location = 1) out vec3  gNormal;
layout(location = 2) out vec4  gAlbedo;
layout(location = 3) out vec3  gPicker;
layout(location = 4) out vec4  gBRDF;
layout(location = 5) out vec3  gDepth;

//const float gamma = 2.2; 
uniform float gamma;

void main()
{
  /* Store the fragment position vector in the first gbuffer texture */
  gPosition = posVF.xyz;

  /* Also store the per-fragment normals into the gbuffer */
  gNormal = normalize(nrmVF).xyz;

  vec3 diffuse;

  if(showMetric == 1)
  {
    vec3 Xaxis = vec3(1.0, 0.0, 0.0);
    vec3 Yaxis = vec3(0.0, 1.0, 0.0);
    vec3 nrmWorld = normalize(nrmWF).xyz;

    vec2 t;
    if( abs(dot(nrmWorld, Yaxis)) < 0.01 )
    {
      if( abs(dot(nrmWorld, Xaxis)) < 0.01 )
        t = fract(posWF.xy * 0.5) * 2.0; 
      else 
        t = fract(posWF.zy * 0.5) * 2.0;
    }
    else
      t = fract(posWF.xz * 0.5) * 2.0; 

    vec2 length = { -1.0, -1.0 };
    // compute the mirror effect
    vec2 texC = abs(t - length); 
    diffuse = texture(tex_metric, texC).rgb;
  }
  else 
    diffuse = texture(tex_diffuse, uvCoord).rgb;

  diffuse = pow(diffuse, vec3(gamma));// Gamma correction 

  vec3 ao = texture(tex_ao, uvCoord).rgb;
  ao = pow(ao, vec3(gamma));// Gamma correction 

  /* And the diffuse per-fragment color */
  gAlbedo = vec4(diffuse, 1.0) * tint * vec4(ao, 1.0);

  vec3 glossTex = texture(tex_gloss, uvCoord).rgb;
  glossTex = pow(glossTex, vec3(gamma));// Gamma correction 

  /* Specular per-fragment color */
  gAlbedo.a = glossTex.r;

  /* Picker Obj Color */
  gPicker = pickCol;

  gBRDF.r = roughness;
  gBRDF.g = metalness;

  float depth = posVF.z;
  gDepth = vec3(depth);
}