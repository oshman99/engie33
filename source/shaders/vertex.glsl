#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
layout (location = 2) in vec2 aTexCoord;

uniform float offset;

out vec3 ourColor; // output a color to the fragment shader
out vec2 TexCoord;

void main()
{
    
    gl_Position = vec4(aPos.x - clamp(offset, 0.0f, 1.0f), aPos.y, aPos.z, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
}       