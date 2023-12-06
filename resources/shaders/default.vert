#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 object_space_position;
layout(location = 1) in vec3 object_space_normal;
layout(location = 2) in vec2 vertex_UV; // UV Mapping
layout(location = 3) in vec3 tangent_vector; // Tangent vector for normal mapping

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 world_space_position;
out vec3 world_space_normal;
out vec2 UV; // UV mapping
out vec3 world_space_tangent; // Tangent vector for normal mapping

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 model_matrix;
uniform mat3 inverse_transpose_matrix;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5

    world_space_position =  vec3(model_matrix * vec4(object_space_position,1.0));
    world_space_normal = inverse_transpose_matrix * object_space_normal;
    world_space_tangent = inverse_transpose_matrix * tangent_vector; // Does this also need to be multiplied by inverse transpose?

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(object_space_position,1.0);

    // UV Mapping
    UV = vertex_UV; // Just pass on the UV coordinate to the fragment shader.
}
