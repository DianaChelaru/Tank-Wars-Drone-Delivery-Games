#version 330

// Input
// TODO(student): Get values from vertex shader
in vec3 frag_pos;
in vec2 frag_texture;
in float noise_val;

// Output
layout(location = 0) out vec4 out_color;

uniform int choose_color;

uniform vec3 terrain_color1;
uniform vec3 terrain_color2;


void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(1);
    vec3 color;

    if (choose_color == 0){
        color = mix(terrain_color1, terrain_color2, noise_val);
     }

    out_color = vec4(color, 1);
}
