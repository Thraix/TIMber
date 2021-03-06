//vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec4 vert_color;
out vec2 uv;
out vec3 vert_pos;

uniform vec4 uMaterialColor = vec4(1,1,1,1);
uniform mat4 uTransformationMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

void main()
{
  gl_Position = uProjectionMatrix * uViewMatrix * uTransformationMatrix * vec4(position, 1.0f);
  vert_color = uMaterialColor;
  vert_pos = position;
  uv = 0.5f - position.xz;
}

//fragment
#version 330 core

in vec4 vert_color;
in vec3 vert_pos;
in vec2 uv;

out vec4 out_color;

uniform sampler2D tex;
uniform float uTime;

void main()
{
  float rings = 5;
  float m = mod((asin(normalize(vert_pos).y)) * (rings / 3.14159 * 2), 2);
  if(m > 1)
    m = 2 - m;
    out_color = vert_color*m;
  out_color.a = vert_color.a;
  //out_color = texture(tex, uv);

  return;
}
