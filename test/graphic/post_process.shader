#shader vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
    TexCoord = aTexCoord;
}

#shader fragment
#version 450 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, TexCoord).rgb;

    if(length(color) < 0.001)
        discard;
        
    // Invert color
    FragColor = vec4(1 - color, 1.0);

    // // grey
    // float temp = color.r + color.g + color.b;
    // FragColor = vec4(temp/3, temp/3, temp/3, 1.0);
}