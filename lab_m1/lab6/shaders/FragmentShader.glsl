#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(1);
    out_color = vec4(frag_normal, 1);
}
