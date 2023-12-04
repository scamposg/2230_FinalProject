#include "glrenderer.h"

glm::mat4 GLRenderer::get_view_matrix() {

    //create Mtranslate from inverse m_camera_position
    glm::vec4 col0(1.0,0.0,0.0,0.0);
    glm::vec4 col1(0.0,1.0,0.0,0.0);
    glm::vec4 col2(0.0,0.0,1.0,0.0);
    glm::vec4 col3(-m_camera_pos[0],-m_camera_pos[1],-m_camera_pos[2],1.0);

    glm::mat4 matTranslate(col0,col1,col2,col3);

    //get w, v , u vectors
    glm::vec3 w(-m_camera_look[0],-m_camera_look[1],-m_camera_look[2]);
    w = glm::normalize(w);
    glm::vec3 v = glm::normalize(m_camera_up - glm::dot(m_camera_up,w)*w);
    glm::vec3 u = glm::cross(v,w);

    //create Mrotate
    glm::vec4 colr0(u[0],v[0],w[0],0.0);
    glm::vec4 colr1(u[1],v[1],w[1],0.0);
    glm::vec4 colr2(u[2],v[2],w[2],0.0);
    glm::vec4 colr3(0.0,0.0,0.0,1.0);

    glm::mat4 matRotate(colr0,colr1,colr2,colr3);

    //multiply Mrotate and Mtranslate
    return matRotate*matTranslate;
}

glm::mat4 GLRenderer::get_proj_matrix(){

    float near = m_near_plane;
    float far = m_far_plane;
    float theta_w = m_camera_width;
    float theta_h = m_camera_height;
    float c = -near/far;
    glm::mat4 z_remap(1.f,0.f,0.f,0.f,
                      0.f,1.f,0.f,0.f,
                      0.f,0.f,-2.f,0.f,
                      0.f,0.f,-1.f,1.f);
    glm::mat4 unhinge(1.f,0.f,0.f,0.f,
                      0.f,1.f,0.f,0.f,
                      0.f,0.f,1.f/(1.f+c),-1.f,
                      0.f,0.f,-c/(1.f+c),0.f);
    glm::mat4 scale(1.f/(far*tan(theta_w/2.f)),0.f,0.f,0.f,
                    0.f,1.f/(far*tan(theta_h/2.f)),0.f,0.f,
                    0.f,0.f,1.f/far,0.f,
                    0.f,0.f,0.f,1.f);
    return z_remap*unhinge*scale;
}
