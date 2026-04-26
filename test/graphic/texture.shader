#shader vertex
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    v_TexCoord = TexCoord;
}

#shader fragment
#version 450 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_mainTexture;

void main()
{
    FragColor = texture(u_mainTexture, v_TexCoord);
}