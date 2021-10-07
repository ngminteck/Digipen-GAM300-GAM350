#version 430
//forward Lighting

/*  This is required for GLSL below 4.3 to explicitly define uniform variable locations */
#extension GL_ARB_explicit_uniform_location : require

#define MAX_LIGHT 150
struct LightData 
{
  vec4 position;
  vec4 color;
  float intensity;
  float radius;
  vec2 pad;
};

layout( binding = 0, std140 ) uniform LightBlock
{
    LightData lights[MAX_LIGHT];
};

in vec4 posVF;
in vec4 posWF;
in vec4 nrmVF;
in vec4 nrmWF;
in vec2 uvCoord;
in vec4 posCS;

uniform mat4 viewMat;
uniform int numLights;

uniform sampler2D tex_diffuse;
uniform sampler2D tex_ao;
uniform sampler2D tex_gloss;
uniform sampler2D tex_normal;
uniform vec4 tint;
uniform float roughness;
uniform float metalness;
uniform float exposure;

uniform int showMetric;
uniform sampler2D tex_metric;

uniform samplerCube tex_Cube;

out vec4 outColor;

const float PI = 3.14159265;
const float shininess = 16.0;
const float k = 0.2;
const float F0 = 0.45;
//const float gamma = 2.2; 
uniform float gamma;

//Constant Sky Color should replace with data from environment map
const vec3  Ka = vec3(86.0, 184.0, 255.0) / 510.0; //Cornflower blue
//const vec3  Ka = vec3(1.0, 1.0, 1.0); //Cornflower blue
const float ambientPow = 0.2f;

float SchlickFunc( float vDoth )
{
  float fresnel = pow(1.0 - vDoth, 5.0);
  fresnel *= (1.0 - F0);
  return fresnel + F0;
}

float GeometricFunc( float NDotH, float NDotV, float NDotL, float VDotH )
{
  // geometric attenuation
  float nh2 = 2.0 * NDotH;
  float g1 = (nh2 * NDotV) / VDotH;
  float g2 = (nh2 * NDotL) / VDotH;
  return min(1.0, min(g1, g2));
}

vec3 BlendMaterial(vec3 Kdiff, vec3 Kspec, vec3 Kbase, float metalness)
{
  float range = mix(0.2, 0.45, metalness);
  vec3  dielectric = Kdiff + Kspec;
  vec3  metal = Kspec * Kbase;
  return mix(dielectric, metal, metalness);
}

void main()
{
  int num_lights = numLights > MAX_LIGHT ? MAX_LIGHT : numLights;

  vec3 texDiff;
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
    texDiff = texture(tex_metric, texC).rgb;
  }
  else 
    texDiff = texture(tex_diffuse, uvCoord).rgb;

  float alpha = texture(tex_diffuse, uvCoord).a;

  // Gamma correction 
  texDiff = pow(texDiff, vec3(gamma));

  vec3 ao = texture(tex_ao, uvCoord).rgb;
  // Gamma correction 
  ao = pow(ao, vec3(gamma));

  vec3 glossTex = texture(tex_gloss, uvCoord).rgb;
  // Gamma correction 
  glossTex = pow(glossTex, vec3(gamma));

  float specCol = glossTex.r;

  vec3 diffuse = (vec4(texDiff, 1.0) * tint * vec4(ao, 1.0)).rgb;
  float rough = mix(0.0, 0.25, roughness);

  vec3 viewDir = normalize(-posVF.xyz);
  vec3 vNrm = normalize(nrmVF.xyz);

  vec3 totalKd = vec3(0.0, 0.0, 0.0);
  vec3 totalKs = vec3(0.0, 0.0, 0.0);

  if(alpha > 0.0f)
  {
    for(int i = 0; i < num_lights; ++i)
    {
      if(lights[i].position.w >= 0.0)
      {
        vec3 lCol = lights[i].color.rgb;
        //Convert light position to view frame
        vec3 lightPos = (viewMat * lights[i].position).xyz;

        //Light vector in view frame
        vec3 lightDir;
        bool isPointLight = (lights[i].position.w == 1.0);

        if(isPointLight) lightDir = lightPos - posVF.xyz;
        else lightDir = lightPos;

        float lDist = length(lightDir);
        //Normalize Light Vector
        lightDir = lightDir / lDist;

         float attenuation = 1.0;
        if(isPointLight) //Point Light
        {
          attenuation = lights[i].radius / lDist; 
          attenuation = clamp(attenuation, 0.0, 1.0) * lights[i].intensity;
        }
        else attenuation = lights[i].intensity;

        //Diffuse
        float nDotl = max(dot(vNrm, lightDir), 0.0);

        if(nDotl > 0.0)
        {
          vec3 hV = normalize(lightDir + viewDir);
          float nDoth = max(dot(vNrm, hV), 0.0);
          float nDotv = max(dot(vNrm, viewDir), 0.0);
          float vDoth = max(dot(viewDir, hV), 0.0);
          float roughSq = max(pow(rough, 2), 0.001);
          float nDoth2 = nDoth * nDoth;

          // geometric attenuation
          float geoAtt = GeometricFunc( nDoth, nDotv, nDotl, vDoth );

          // roughness / microfacet distribution
          float r1 = 1.0 / ( 4.0 * roughSq * pow(nDoth, 4.0));
          float r2 = (nDoth2 - 1.0) / (roughSq * nDoth2);
          float roughVal = r1 * exp(r2);

          //fresnel Schlick
          float fresnelVal = SchlickFunc( vDoth );

          float spec = (fresnelVal * geoAtt * roughVal) / (PI);

          vec3 Kd = lCol * nDotl * attenuation;
          vec3 Ks = lCol * nDotl * (spec * specCol) * attenuation;

          totalKd += Kd;
          totalKs += Ks;
        }
      }
    }
  }

  vec3 finalCol = BlendMaterial(totalKd ,totalKs, diffuse, metalness);
  finalCol = (finalCol * diffuse);

  const float gamma = 2.2; 
  // Reinhard tone mapping
  //finalCol = finalCol / (finalCol + vec3(1.0));
  finalCol = vec3(1.0) - exp(-finalCol * exposure);
  // Gamma correction 
  finalCol = pow(finalCol, vec3(1.0 / gamma));
  
  outColor = vec4(finalCol, tint.a * alpha);
}