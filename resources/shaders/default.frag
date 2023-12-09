#version 330 core

in vec3 world_space_position;
in vec3 world_space_normal;


// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

uniform vec3 object_ambient;
uniform float k_a;

uniform vec3 object_diffuse;
uniform float k_d;
uniform vec3 world_space_light_direction;

uniform vec3 object_specular;
uniform float k_s;
uniform vec3 camera_position;
uniform float shininess;

// UV Mapping
in vec2 UV;
uniform float k_t;
uniform sampler2D objectTexture;

// Normal Mapping

void main() {
    vec3 ambient = vec3(k_a*object_ambient);
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    // add diffuse component to output color
    //calculate surface to light ray
    vec4 surface_to_light_ray = normalize(vec4(-world_space_light_direction,0.f));
    vec4 world_space_normal_ray = normalize(vec4(world_space_normal,0.f));
    // diffuse = kd * dot ( normal, surface to light )
    float diffuse_theta = clamp(dot(world_space_normal_ray,surface_to_light_ray),0.f,1.f);
    diffuse += k_d*object_diffuse*diffuse_theta;


    // specular = ks * dot (Reflect, surface to camera)^shininess
    vec4 reflected_ray = normalize(reflect(-surface_to_light_ray,world_space_normal_ray));
    vec4 surface_to_camera_ray = normalize(vec4(camera_position-world_space_position,0.f));
    if (shininess>0){
        float specular_theta = pow(clamp(dot(reflected_ray,surface_to_camera_ray),0.f,1.f),shininess);
        specular += k_s*object_specular*specular_theta;
    }
    // UV Mapping

    fragColor = texture(objectTexture, UV);
    fragColor += vec4(ambient + diffuse + specular,0.f);
    fragColor[0] = clamp(fragColor[0],0.f,1.f);
    fragColor[1] = clamp(fragColor[1],0.f,1.f);
    fragColor[2] = clamp(fragColor[2],0.f,1.f);


}
