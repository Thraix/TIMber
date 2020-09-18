//vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 color;
layout(location = 3) in vec3 normal;

out vec4 vert_color;
out vec2 vert_texCoord;
out vec3 surfaceNormal;
out vec3 toLightVector;
out vec3 toCameraVector;
out float visibility;

uniform vec3 uCameraPos;
uniform mat4 uTransformationMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

uniform vec3 uLightPosition = vec3(0.0, 0.0, -10.0);
uniform vec4 uMaterialColor = vec4(1,1,1,1);

const float density = 0.007;
const float gradient = 1.5;

void main()
{

	vec4 worldPosition = uTransformationMatrix * vec4(position, 1.0f);
	vec4 positionRelativeToCamera = uViewMatrix * worldPosition;
	gl_Position = uProjectionMatrix * positionRelativeToCamera;
	vert_color = uMaterialColor * vec4(color.b, color.g, color.r, color.a);

	vert_texCoord = texCoord;
	surfaceNormal = (uTransformationMatrix * vec4(normal, 0.0)).xyz;
	toLightVector = uLightPosition - worldPosition.xyz;
	toCameraVector = uCameraPos - worldPosition.xyz;

	float distance = length(positionRelativeToCamera.xyz);
	visibility = exp(-pow((distance*density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}

//fragment
#version 330 core

in vec4 vert_color;
in vec2 vert_texCoord;
in vec3 surfaceNormal;
in vec3 toLightVector;
in vec3 toCameraVector;
in float visibility;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_brightColor;

uniform sampler2D textureSampler;
uniform vec3 uLightColor;
uniform vec3 fogColor;
uniform float uHasTexture = 1.0;
uniform float uSpecularExponent = 10.0;
uniform float uSpecularStrength = 1;
uniform float uDiffuseStrength = 1;
uniform float uAmbient = 0.3;

void main()
{
	out_color = vert_color;
	if (uHasTexture > 0.5)
	{
		out_color *= texture(textureSampler, vert_texCoord);
		if (out_color.a < 0.1)
			discard;
	}
	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;

	float uDiffuse = uDiffuseStrength * max(dot(unitNormal, unitLightVector), 0.0);
	float uSpecular = uSpecularStrength * pow(max(dot(reflect(lightDirection, unitNormal), unitVectorToCamera), 0.0f), uSpecularExponent);

	out_color *= vec4((uAmbient + (uDiffuse + uSpecular) * uLightColor), 1.0f);
	out_color = mix(vec4(fogColor.xyz, 1.0), vec4(out_color.rgb, 1.0), visibility);
}
