//vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec4 vert_color;

uniform vec4 lineColor = vec4(1,1,1,1);
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * vec4(position, 1.0f);
	vert_color = lineColor;
}

//fragment
#version 330 core

in vec4 vert_color;
out vec4 out_color;

void main()
{
	out_color = vert_color;
	return;
}
