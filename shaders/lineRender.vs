#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 fs_color;

uniform float transparency;
uniform mat4 projMatrix;

// Renders a series of lines.
void main(void)
{
	fs_color = vec4(color, 1.0f);
    
    gl_Position = projMatrix * vec4(position, transparency);
}
