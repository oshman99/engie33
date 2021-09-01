#version 330 core
out vec4 FragColor;  


//in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture_specular1;


void main()
{
    FragColor = texture(texture_specular1, TexCoord);
}