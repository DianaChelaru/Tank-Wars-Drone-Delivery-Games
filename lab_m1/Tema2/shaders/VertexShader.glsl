#version 330
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;

out vec3 frag_pos;
out vec2 frag_texture;
out float noise_val;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

float random (in vec2 st){
	return fract(sin(dot(st.xy,vec2(12.9898,78.233)))* 43758.5453123);
}

float noise (in vec2 st) {
	vec2 i = floor(st);
	vec2 f = fract(st);

	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));

	vec2 u = f*f*(3.0-2.0*f);
	return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main(){
	vec3 pos = v_pos;

	float frequency = 0.3;
	float amplitude = 1;
	noise_val = noise(v_pos.xz * frequency) * amplitude;
	pos.y += noise_val;

	frag_pos = pos;
	frag_texture = pos.xz;

	gl_Position = Projection * View * Model * vec4(frag_pos, 1.0);
}