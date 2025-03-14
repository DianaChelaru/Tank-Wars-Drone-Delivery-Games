#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 light_position2;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// Uniforms for spot light properties
uniform float cutOff; // Angle of the spot light in radians

uniform int light_type; // 0: Point light, 1: Spot light

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

vec3 point_light_contribution(vec3 light_pos, vec3 light_color) {
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_pos - world_position);
    vec3 V = normalize(eye_position - world_position);

    float ambient_light = 0.25 * material_kd;
    float diffuse_light = material_kd * max(dot(N, L), 0.0);
    float specular_light = 0;

    if (diffuse_light > 0) {
        vec3 H = normalize(L + V);
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    float light = 0.f;

    if (light_type == 0) {
        // Point light attenuation
        float dist = distance(light_pos, world_position);
        float att_factor = 1.f / max(dist * dist, 1.f);
        light = ambient_light + att_factor * (diffuse_light + specular_light);
    } else {
        // Spot light calculation
        light = ambient_light;

        float spot_light = dot(-L, light_direction);
        float spot_light_limit = cos(cutOff); // Use the uniform cutOff
        if (spot_light > spot_light_limit) {
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);
            float dist = distance(light_pos, world_position);
        float att_factor = 1.f / max(dist * dist, 1.f);
            light += light_att_factor * att_factor;
        }
    }

    return light_color * light * object_color;
}

void main() {
    // Compute contributions from two light sources
    vec3 light1 = point_light_contribution(light_position, vec3(1, 1, 1) - light_color);
    vec3 light2 = point_light_contribution(light_position2, light_color);

    // Output the final color
    out_color = vec4(light1 + light2, 1.f);
}
