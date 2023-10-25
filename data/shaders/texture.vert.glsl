layout (location = 0) in vec4 vertex;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

uniform mat4 modelViewMatrix;

out vec2 v_uv;
out vec4 v_color;

void main()
{
    v_uv = uv;
    v_color = color;
    gl_Position = modelViewMatrix * vertex;
}
