#version 330

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_texture;
in vec3 frag_color;

layout(location = 0) out vec4 out_color;

uniform int choose_color;

uniform vec3 drone_bodyColor;
uniform vec3 done_elliceColor;
uniform vec3 tree_trunkColor;
uniform vec3 tree_leavesColor1;
uniform vec3 tree_leavesColor2;
uniform vec3 package_color;
uniform vec3 dest_color;
uniform vec3 arrow_color;

void main(){
	out_color = vec4(1);

	vec3 color;

     if (choose_color == 1) {
        color = drone_bodyColor;
     }
     if (choose_color == 2) {
        color = done_elliceColor;
     }
     if (choose_color == 3) {
        color = tree_trunkColor;
     }
     if (choose_color == 4) {
        color = tree_leavesColor1;
     }
     if (choose_color == 5) {
        color = package_color;
     }
     if (choose_color == 6) {
        color = dest_color;
     }
     if (choose_color == 7) {
        color = arrow_color;
     }
     if (choose_color == 8) {
        color = tree_leavesColor2;
     }

	out_color = vec4(color, 1);
}