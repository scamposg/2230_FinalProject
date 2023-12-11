#version 330 core

layout(location = 0) in vec3 object_space_position;

// Values that stay constant for the whole mesh.
uniform mat4 shadow_depth_matrix;

out vec4 shadow_space_position;

void main(){
 shadow_space_position =  shadow_depth_matrix * vec4(object_space_position,1);
}
