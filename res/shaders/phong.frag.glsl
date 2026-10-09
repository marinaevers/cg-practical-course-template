// Fragment shader for all primitives: Blinn-Phong lighting with one point light and one spotlight,
// optional base texture and an optional animated overlay texture.

in vec3 world_position;
in vec3 world_normal;
in vec2 texcoord;

out vec4 frag_color;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool emissive;      // unlit, e.g. the lamp
};

struct PointLight {
    vec3 position;
    vec3 color;         // color multiplied by intensity
    float attenuation;  // quadratic attenuation factor
};

struct SpotLight {
    vec3 position;
    vec3 direction;     // normalized, pointing away from the light
    vec3 color;
    float cos_outer;    // cosine of the cutoff angle
    float cos_inner;    // cosine of the angle where the soft edge starts
    float attenuation;
};

uniform Material material;
uniform vec3 ambient_light;
uniform PointLight point_light;
uniform vec3 camera_position;

uniform bool has_texture;
uniform sampler2D base_texture;     // texture unit 0

// Diffuse + specular contribution of one light (Blinn-Phong).
// n: surface normal, v: direction to the viewer, l: direction to the light (all normalized).
vec3 blinn_phong(vec3 n, vec3 v, vec3 l, vec3 radiance, vec3 kd)
{
    float n_dot_l = dot(n, l);
    if (n_dot_l <= 0.0)
        return vec3(0.0); // surface faces away from the light
    vec3 h = normalize(l + v); // half vector between light and view direction
    float specular = pow(max(dot(n, h), 0.0), material.shininess);
    return radiance * (kd * n_dot_l + material.specular * specular);
}

void main(void)
{
    vec3 kd = material.diffuse;

    vec3 color;
    vec3 n = normalize(world_normal);
    // Two-sided lighting: open surfaces (rings, the inside of the roof) are visible from both sides.
    // On the back side the normal has to be flipped, otherwise these faces would always be dark.
    if (!gl_FrontFacing)
        n = -n;
    vec3 v = normalize(camera_position - world_position);

    // Ambient term: low because the scene takes place at night
    color = ambient_light * kd;

    // No shadows: every light reaches every surface that faces it.
    // Point light of the lamp, attenuated with the distance
    vec3 to_light = point_light.position - world_position;
    float d = length(to_light);
    float attenuation = 1.0 / (1.0 + point_light.attenuation * d * d);
    color += blinn_phong(n, v, to_light / d, point_light.color * attenuation, kd);

    // TODO: Add Spotlight

    frag_color = vec4(color, 1.0);
}
