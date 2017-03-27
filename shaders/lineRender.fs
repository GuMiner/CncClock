#version 400 core

out vec4 color;
in vec4 fs_color;

// Pass-thru the assigned color from the underlying shader.
void main(void)
{
	color = fs_color;
}