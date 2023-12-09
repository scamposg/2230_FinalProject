#include "glrenderer.h"

void GLRenderer::apply_bezier_matrices(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3){

    for (int i=0; i<m_road_matrices.size(); i++){
        float t = m_road_z_buffer[i]/-m_radius;
        glm::vec3 curve = std::pow(1.f-t,3.f) * P0 + 3.f*std::pow(1.f-t,2.f)*t*P1 +
                          3.f*(1.f-t)*t*t*P2 + t*t*t*P3;
        t= t+(1.f/-m_radius);
        glm::vec3 next_curve = std::pow(1.f-t,3.f) * P0 + 3.f*std::pow(1.f-t,2.f)*t*P1 +
                               3.f*(1.f-t)*t*t*P2 + t*t*t*P3;
        glm::vec3 ray = glm::cross(next_curve-curve,glm::vec3(-1,0,0));
        glm::vec3 up(0,1,0);
        float theta = acos(glm::dot(ray,up)/abs(glm::length(ray)*glm::length(up)));
        glm::vec4 prior_pos = m_original_road_matrices[i]*glm::vec4(1);
        m_road_matrices[i] = m_original_road_matrices[i]*glm::translate(glm::vec3(0,curve.y-prior_pos.y,curve.z-prior_pos.z))*glm::rotate(-theta,glm::vec3(1,0,0));
    }
    P0 = P0 + glm::vec3(0,0.75,0.75);
    P1 = P1 + glm::vec3(0,0.75,0.75);
    P2 = P2 + glm::vec3(0,0.75,0.75);
    P3 = P3 + glm::vec3(0,0.75,0.75);
    for (int i=0; i<m_building_matrices.size(); i++){
        float t = m_building_z_buffer[i]/-m_radius;
        glm::vec3 curve = std::pow(1.f-t,3.f) * P0 + 3.f*std::pow(1.f-t,2.f)*t*P1 +
                          3.f*(1.f-t)*t*t*P2 + t*t*t*P3;
        t= t+(1.f/-m_radius);
        glm::vec3 next_curve = std::pow(1.f-t,3.f) * P0 + 3.f*std::pow(1.f-t,2.f)*t*P1 +
                               3.f*(1.f-t)*t*t*P2 + t*t*t*P3;
        glm::vec3 ray = glm::cross(next_curve-curve,glm::vec3(-1,0,0));
        glm::vec3 up(0,1,0);
        float theta = acos(glm::dot(ray,up)/abs(glm::length(ray)*glm::length(up)));
        glm::vec4 prior_pos = m_original_building_matrices[i]*glm::vec4(1);
        m_building_matrices[i] = m_original_building_matrices[i]*glm::translate(glm::vec3(0,curve.y-prior_pos.y,curve.z-prior_pos.z))*glm::rotate(-theta,glm::vec3(1,0,0));
    }

    return;
}

void GLRenderer::play_scene(){
    if (m_to_play) m_to_play = false;
    else m_to_play = true;
}

void GLRenderer::reset_scene(){
    m_to_play = false;
    m_road_matrices = m_original_road_matrices;
    m_building_matrices = m_original_building_matrices;
    m_curve_0 = m_curve_OG_0;
    m_curve_1 = m_curve_OG_1;
    m_curve_2 = m_curve_OG_2;
    m_curve_3 = m_curve_OG_3;
    apply_bezier_matrices(m_curve_OG_0,m_curve_OG_1,m_curve_OG_2,m_curve_OG_3);

    update();
}
