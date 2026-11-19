#shader vertex
#version 410 core
layout(location = 0) in vec2 position;
uniform vec4 box;
void main() { gl_Position = vec4(position * box.zw + box.xy, 0.0, 1.0); }
#shader fragment
#version 410 core
uniform vec3 tint;
out vec4 color;
void main() { color = vec4(tint, 1.0); }
