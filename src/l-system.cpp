#include "glrenderer.h"

enum direction {
    back,left,front,right
};

void create_l_system_string(std::vector<char> &previous, int recurse_count){
    if (recurse_count == 0) return;
    char x_string[] = {'B','L','B','[','X','B',']','L','B'};

    for (int i = 0; i<size(previous); i++){
        if (previous.at(i) == 'X'){
            std::vector<char> new_string;
            for (int k=0;k<i;k++){
                new_string.push_back(previous[k]);
            }
            for (int j = 0; j<std::size(x_string); j++){
                new_string.push_back(x_string[j]);
            }
            for (i=i+1; i<size(previous);i++){
                new_string.push_back(previous[i]);
            }
            previous = new_string;
            break;
        }
    }
    std::vector<char> final_string;
    for (int i=0; i<size(previous); i++){
        if (previous.at(i) == 'B'){
            final_string.push_back('L');
            final_string.push_back('B');
            final_string.push_back('B');
        }
        else {
            final_string.push_back(previous[i]);
        }
    }
    previous=final_string;

    recurse_count --;
    create_l_system_string(previous, recurse_count);
    return;
}

glm::mat4 go_in_direction(direction dir){
    if (dir == front) return glm::translate(glm::vec3(0,0,2));
    if (dir == right) return glm::translate(glm::vec3(2,0,0));
    if (dir == back) return glm::translate(glm::vec3(0,0,-2));
    if (dir == left) return glm::translate(glm::vec3(-2,0,0));
}

direction inv_dir(direction dir){
    if (dir == front) return right;
    if (dir == right) return back;
    if (dir == back) return left;
    if (dir == left) return front;
}


direction switch_dir(direction dir){
    if (dir == front) return left;
    if (dir == left) return back;
    if (dir == back) return right;
    if (dir == right) return front;
}

void get_city_matrices(std::vector<glm::mat4> &matrices, glm::mat4 building_scale){
    std::vector<char> l_string;
    l_string.push_back('X');
    create_l_system_string(l_string,10);
    std::vector<glm::mat4> prior_ctms;
    glm::mat4 current_ctm(1);
    float theta = glm::radians(90.f);
    prior_ctms.push_back(current_ctm);
    prior_ctms.push_back(current_ctm);
    direction dir = back;

    for (int i=0; i<size(l_string); i++){
        if (l_string[i] == 'B'){
            glm::mat4 new_ctm = prior_ctms.back()*go_in_direction(dir);
            if (new_ctm.length() < 50){
            current_ctm = new_ctm;
            prior_ctms.back() = current_ctm;
            matrices.push_back(current_ctm);
            }
            continue;
        }
        if (l_string[i] == 'R'){
            dir = switch_dir(dir);
            continue;
        }
        if (l_string[i] == 'L'){
            dir = inv_dir(dir);
            continue;
        }
        if (l_string[i] == '['){
            prior_ctms.push_back(current_ctm);
            continue;
        }
        if (l_string[i] == ']'){
            prior_ctms.pop_back();
            continue;
        }
    }
    return;
}

void GLRenderer::generate_city(){
    std::vector<glm::mat4> matrices;
    glm::mat4 scale = glm::scale_slow(glm::mat4(1),glm::vec3(1,1,4));
    get_city_matrices(matrices,scale);

    m_matrices = matrices;
    update();
}
