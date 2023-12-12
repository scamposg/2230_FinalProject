#version 330 core

layout(location = 0) in vec3 object_space_position;

// Values that stay constant for the whole mesh.
uniform mat4 shadow_depth_matrix;

void main(){
 gl_Position =  shadow_depth_matrix * vec4(object_space_position,1);
}
