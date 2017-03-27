#version 400 core
// Pulled from IMGUI_IMPL_GLFW_GL3.CPP

uniform sampler2D Texture;

in vec2 Frag_UV;
in vec4 Frag_Color;
out vec4 Out_Color;

void main()
{
    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
}