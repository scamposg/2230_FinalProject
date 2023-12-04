#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 world_space_position;
in vec3 world_space_normal;

// Task 10: declare an out vec4 for your output color
out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float k_a;

// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float k_d;
uniform vec3 world_space_light_position;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float k_s;
uniform vec3 camera_position;
uniform float shininess;

// UV Mapping
in vec2 UV;
uniform float k_t;
uniform sampler2D objectTexture;

// Normal Mapping

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
//    fragColor = vec4(1.0);

    // Task 11: set your output color to the absolute value of your world-space normals,
    //          to make sure your normals are correct.
//    fragColor = vec4(abs(world_space_normal), 1.0);

    // Task 12: add ambient component to output color
    float ambient = k_a;
    // Task 13: add diffuse component to output color
    //calculate surface to light ray
    vec4 surface_to_light_ray = normalize(vec4(world_space_light_position,0.f) - vec4(world_space_position, 0.f));
    vec4 world_space_normal_ray = normalize(vec4(world_space_normal,0.f));
    // diffuse = kd * dot ( normal, surface to light )
    float diffuse = k_d * clamp(dot(world_space_normal_ray, surface_to_light_ray),0.f,1.f);
//    fragColor = vec4(ambient + diffuse,ambient + diffuse,ambient + diffuse,1.f);

    // Task 14: add specular component to output color
    vec4 reflected_ray = normalize(reflect(-surface_to_light_ray,world_space_normal_ray));
    vec4 surface_to_camera_ray = normalize(vec4(camera_position-world_space_position,0.f));

    // specular = ks * dot (Reflect, surface to camera)^shininess
    float specular = k_s * pow(clamp(dot(reflected_ray,surface_to_camera_ray),0.f,1.f),shininess);

    // UV Mapping
    vec3 textureColor = texture(objectTexture, UV).rgb * k_t;

    float sum = ambient + diffuse + specular;

    fragColor = vec4(sum,sum,sum,1.f);

}
