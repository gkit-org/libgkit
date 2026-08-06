#shader vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 v_Color;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    v_Color = aColor;
}

#shader fragment
#version 450 core
out vec4 FragColor;

in vec3 v_Color;

// Alpha factor for the blend test; defaults to opaque so an unset uniform
// does not silently make the object invisible.
uniform float u_alpha = 1.0;

void main()
{
    FragColor = vec4(v_Color, u_alpha);
}
