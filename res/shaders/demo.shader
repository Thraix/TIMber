//vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

out VertexData
{
  vec3 worldPos;
  vec4 color;
  float visibility;
} vs_out;

uniform mat4 uTransformationMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
const float density = 0.003;
const float gradient = 1.5;

void main()
{
  vs_out.color = color;
  vec4 worldPosition = uTransformationMatrix * vec4(position, 1.0f);
  vec4 positionRelativeToCamera = uViewMatrix * worldPosition;

  gl_Position = uProjectionMatrix * positionRelativeToCamera;
  vs_out.worldPos = worldPosition.xyz;

  float distance = length(positionRelativeToCamera.xyz);
  vs_out.visibility = exp(-pow((distance*density), gradient));
  vs_out.visibility = clamp(vs_out.visibility, 0.0, 1.0);
}

//geometry
#version 330 core 

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform vec3 uCameraPos;
uniform mat4 uTransformationMatrix;
uniform mat4 uViewMatrix;

uniform vec3 light_color = vec3(1.0f, 0.96f, 0.9f);
uniform sampler2D noiseTexture;


in VertexData
{
  vec3 worldPos;
  vec4 color;
  float visibility;
} gs_in[];

out FragmentData
{
  vec4 color;
  float visibility;
} gs_out;

vec3 rand(in vec3 pos)
{
  vec3 pos2;
  vec2 uv1 = vec2(pos.x, pos.x);
  vec2 uv2 = vec2(pos.y, pos.y);
  vec2 uv3 = vec2(pos.z, pos.z);
  pos2.x = (texture(noiseTexture, uv1).r-0.5f)*1;
  pos2.y = (texture(noiseTexture, uv2).r-0.5f)*1;
  pos2.z = (texture(noiseTexture, uv3).r-0.5f)*1;
  return pos2;
}

void main()
{
  vec3 position = (gs_in[0].worldPos + gs_in[1].worldPos + gs_in[2].worldPos) / 3.0;
  vec3 surfaceNormal = normalize(cross(
        gs_in[1].worldPos - gs_in[0].worldPos, 
        gs_in[2].worldPos - gs_in[0].worldPos));// + rand(position);
  vec3 toLightVector = vec3(100, 100, 0);
  vec3 toCameraVector = uCameraPos - position;
  vec3 unitNormal = normalize(surfaceNormal);
  vec3 unitLightVector = normalize(toLightVector);

  float nDot = dot(unitNormal, unitLightVector);
  float brightness = (nDot+1) / 2.0;//max(nDot,0.6);

  if (brightness < 0.7)
    brightness = mix(0.3, 0.7, brightness / 0.7);

  vec3 diffuse = light_color * brightness;
  vec4 color = gs_in[0].color * vec4(diffuse, 1.0f);

  for(int i = 0; i<3;i++)
  {
    gl_Position = gl_in[i].gl_Position;
    gs_out.visibility = gs_in[i].visibility;
    gs_out.color = vec4((unitNormal+1) * 0.5,1.0);//gs_in[i].color * vec4(diffuse, 1.0f);

    EmitVertex();
  }
  EndPrimitive();
}


//fragment
#version 330 core

flat in vec4 vert_color;
flat in float visibility;

in FragmentData
{
  vec4 color;
  float visibility;
} fs_in;


out vec4 out_color;
uniform vec4 fogColor = vec4(0.125, 0.125, 0.125, 1);

void main()
{
  out_color = fs_in.color;
  out_color = mix(vec4(fogColor.rgb, out_color.a), vec4(out_color.rgb, out_color.a), fs_in.visibility);
}
