#version 330 core

in vec3 world_space_position;
in vec3 world_space_normal;
in vec3 world_space_tangent;
in vec2 UV;

out vec4 fragColor;

uniform float k_a;
uniform float k_d;
uniform float k_s;
uniform float shininess;
uniform vec3 world_space_light_position;
uniform vec3 camera_position;
uniform sampler2D objectTexture;
uniform sampler2D normalMap;

vec4 CalcBumpedNormal()
{
    vec3 Normal = normalize(world_space_normal);
    vec3 Tangent = normalize(world_space_tangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalMap, UV).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return vec4(NewNormal, 0.0f);
}

void main() {
    float ambient = k_a;
    vec4 bumpedNormal = CalcBumpedNormal();
    //vec4 bumpedNormal = normalize(vec4(world_space_normal,0.f));
    vec4 surface_to_light_ray = normalize(vec4(world_space_light_position,0.f) - vec4(world_space_position, 0.f));
    // diffuse = kd * dot ( normal, surface to light )
    float diffuse = k_d * clamp(dot(bumpedNormal, surface_to_light_ray),0.f,1.f);
    vec4 reflected_ray = normalize(reflect(-surface_to_light_ray,bumpedNormal));
    vec4 surface_to_camera_ray = normalize(vec4(camera_position-world_space_position,0.f));
    // specular = ks * dot (Reflect, surface to camera)^shininess
    float specular = k_s * pow(clamp(dot(reflected_ray,surface_to_camera_ray),0.f,1.f),shininess);

    // UV Mapping
    vec3 textureColor = texture(objectTexture, UV).rgb;
    //vec3 textureColor = texture(normalMap, UV).rgb;

    float sum = ambient + diffuse + specular;
    float sumr = textureColor.x + sum;
    float sumg = textureColor.y + sum;
    float sumb = textureColor.z + sum;

//    float sumr = UV.x;
//    float sumg = UV.y;
//    float sumb = 0.0f;

    fragColor = vec4(sumr, sumg, sumb, 1.f);
}

