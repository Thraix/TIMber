//vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vert_color;
out vec2 uv;

uniform vec4 mat_color = vec4(1,1,1,1);
uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(position, 1.0f);
	vert_color = mat_color ;
  uv = 0.5f - position.xz;
}

//fragment
#version 330 core

in vec4 vert_color;
in vec2 uv;
out vec4 out_color;
uniform sampler2D tex;

void main()
{
	out_color = vert_color * texture(tex, uv);
  if(out_color.a < 0.1)
    discard;
  out_color.a = 1.0f;
	return;
}
