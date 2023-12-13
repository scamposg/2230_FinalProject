#include "glrenderer.h"

enum direction {
    back,left,front,right
};

void create_l_system_string(std::vector<char> &previous, int recurse_count){
    if (recurse_count == 0) return;
    char x_string[] = {'B','B','L','B','[','X','B','B',']','L','B'};

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

glm::mat4 scale_building(direction dir){
    if (dir == front) return glm::scale(glm::vec3(1,1,1));
    if (dir == right) return glm::scale(glm::vec3(1,1,1));
    if (dir == back) return glm::scale(glm::vec3(1,1,1));
    if (dir == left) return glm::scale(glm::vec3(1,1,1));
}

void go_in_direction(direction dir, glm::mat4 &ctm){
    if (dir == front){
        ctm = ctm*glm::translate(glm::vec3(0,0,1));
        return;
    }
    if (dir == right) {
        ctm = ctm*glm::translate(glm::vec3(1,0,0));
        return;
    }
    if (dir == back){
        ctm = ctm*glm::translate(glm::vec3(0,0,-1));
        return;
    }
    if (dir == left) {
        ctm = ctm * glm::translate(glm::vec3(-1,0,0));
        return;
    }
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

int get_map_loc(int x, int z, int radius){
    x = x + (radius/2);
    int index = ((abs(z)*radius) + x);
    if ( index > radius*radius-1){
        return radius*radius-1;
    }
    return index;
}

void place_buildings(direction dir,glm::mat4 ctm, std::vector<glm::mat4> &building_transforms){
    if (dir == front || dir == back){
        building_transforms.push_back(ctm*glm::translate(glm::vec3(-1,0.5,0)));
//        building_transforms.push_back(ctm*glm::translate(glm::vec3(1,0,0)));
    }
    else {
//        building_transforms.push_back(ctm*glm::translate(glm::vec3(0,0,1)));
        building_transforms.push_back(ctm*glm::translate(glm::vec3(0,0.5,-1)));
    }
    return;
}

bool within_bounds(glm::vec3 loc, float radius){
    if (loc.x < -radius/2.f || loc.z < -radius ||
        loc.x > radius/2.f || loc.z > 0) return false;
    return true;
}

void GLRenderer::get_city_matrices(){
    std::vector<char> l_string;
    l_string.push_back('X');
    create_l_system_string(l_string,7);

    glm::mat4 current_ctm = glm::mat4(1);
    std::vector<glm::mat4> prior_ctms;
    prior_ctms.push_back(current_ctm);
    direction dir = back;

    for (int i=0; i<std::size(l_string); i++){
        if (l_string[i] == 'B'){
            glm::mat4 *translation = &current_ctm;
            go_in_direction(dir,*translation);
            std::shared_ptr<glm::mat4> new_ctm = std::make_shared<glm::mat4>
                (*translation);
            glm::vec4 test(0,0,0,1);
            test = *new_ctm*test;
            if (!within_bounds(test,m_radius)){
                dir = switch_dir(dir);
                continue;
            }
            if (m_city_map[get_map_loc(int(test.x),int(test.z),int(m_radius))]){
                current_ctm = *new_ctm;
                continue;
            }
            if (m_road_matrices.size() > 500){
                break;
            }
            m_city_map[get_map_loc(int(test.x),int(test.z),int(m_radius))] = 1;
            current_ctm = *new_ctm;
            std::shared_ptr<float> z = std::make_shared<float>(test.z);
            m_road_z_buffer.push_back(z);
            m_road_matrices.push_back(new_ctm);

            if (test.z < furthest_z) furthest_z = test.z;
            if (test.z > closest_z) closest_z = test.z;

            std::vector<glm::mat4> building_transforms;
            place_buildings(dir,current_ctm, building_transforms);
            for (auto transform : building_transforms){
                test = glm::vec4(0,0,0,1);
                test = transform*test;
                if (m_city_map[get_map_loc(int(test.x),int(test.z),int(m_radius))])
                    continue;
                m_building_matrices.push_back(std::make_shared<glm::mat4>(transform));
                m_building_z_buffer.push_back(std::make_shared<float>(test.z));
                m_city_map[get_map_loc(int(test.x),int(test.z),int(m_radius))] = 1;
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
            current_ctm = prior_ctms.back();
            continue;
        }
    }

    for (int j=0; j<m_city_map.size(); j++){
        if (m_city_map[j] != 1){
            float x = float(j % int(m_radius)) - (m_radius/2.f);
            float z = -std::floor(float(j)/m_radius);
            glm::mat4 grass = glm::translate(glm::vec3(x,0,z));
            m_grass_matrices.push_back(std::make_shared<glm::mat4>(grass));
            m_grass_z_buffer.push_back(std::make_shared<float>(z));
        }
    }
    return;
}

void GLRenderer::generate_city(){
    m_city_map.clear();
    m_city_map.resize(m_radius*m_radius,0);

    m_road_matrices.clear();
    m_building_matrices.clear();

    m_road_z_buffer.clear();
    m_building_z_buffer.clear();
    get_city_matrices();
    m_original_road_matrices = m_road_matrices;
    m_original_grass_matrices = m_grass_matrices;
    m_original_building_matrices = m_building_matrices;
//    apply_bezier_matrices(m_curve_OG_0,m_curve_OG_1,m_curve_OG_2,m_curve_OG_3);
    update();
}
