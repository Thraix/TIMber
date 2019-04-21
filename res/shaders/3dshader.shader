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

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 light_position = vec3(0.0, 0.0, -10.0);
uniform vec4 mat_color = vec4(1,1,1,1);

const float density = 0.007;
const float gradient = 1.5;

void main()
{

	vec4 worldPosition = transformationMatrix * vec4(position, 1.0f);
	vec4 positionRelativeToCamera = viewMatrix * worldPosition;
	gl_Position = projectionMatrix * positionRelativeToCamera;
	vert_color = mat_color * vec4(color.b, color.g, color.r, color.a);

	vert_texCoord = texCoord;
	surfaceNormal = (transformationMatrix * vec4(normal, 0.0)).xyz;
	toLightVector = light_position - worldPosition.xyz;
	toCameraVector = (inverse(viewMatrix) * vec4(0, 0, 0, 1)).xyz - worldPosition.xyz;

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
uniform vec3 light_color = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 fogColor;
uniform float hasTexture = 1.0;
uniform float specularExponent = 10.0;
uniform float specularStrength = 1;
uniform float diffuseStrength = 1;
uniform float ambient = 0.3;

void main()
{
	out_color = vert_color;
	if (hasTexture > 0.5)
	{
		out_color *= texture(textureSampler, vert_texCoord);
		if (out_color.a < 0.1)
			discard;
	}
	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLightVector);
	vec3 unitVectorToCamera = normalize(toCameraVector);
	vec3 lightDirection = -unitLightVector;

	float diffuse = diffuseStrength * max(dot(unitNormal, unitLightVector), 0.0);
	float specular = specularStrength * pow(max(dot(reflect(lightDirection, unitNormal), unitVectorToCamera), 0.0f), specularExponent);

	out_color *= vec4((ambient + (diffuse + specular) * light_color), 1.0f);
	out_color = mix(vec4(fogColor.xyz, 1.0), vec4(out_color.rgb, 1.0), visibility);
}
