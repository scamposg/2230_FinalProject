#version 330 core

//in vec4 shadow_space_position;

// Ouput data
out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = gl_FragCoord.z;
}
