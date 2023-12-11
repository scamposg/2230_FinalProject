#version 330 core

in vec4 shadow_space_position;

// Ouput data
layout(location = 0) out float shadow_depth;

void main(){
    // Not really needed, OpenGL does it anyway
    shadow_depth = shadow_space_position.z;
}
