#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 object_space_position;
layout(location = 1) in vec3 object_space_normal;
layout(location = 2) in vec2 vertex_UV; // UV Mapping
layout(location = 3) in vec3 tangent_vector; // Tangent vector for normal mapping

out vec3 world_space_position;
out vec3 world_space_normal;
out vec2 UV; // UV mapping
out vec3 world_space_tangent; // Tangent vector for normal mapping

uniform mat4 model_matrix;
uniform mat3 inverse_transpose_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
    world_space_position =  vec3(model_matrix * vec4(object_space_position,1.0));
    world_space_normal = inverse_transpose_matrix * object_space_normal;
    world_space_tangent = inverse_transpose_matrix * tangent_vector;

    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(object_space_position,1.0);

    UV = vertex_UV; // Just pass on the UV coordinate to the fragment shader.

    // Shadow mapping
    mat4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.5f, 0.0f,
                    0.5f, 0.5f, 0.5f, 1.0f);

    glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
}
