#include "glrenderer.h"

glm::mat4 GLRenderer::get_view_matrix() {
    // Optional TODO: implement the getter or make your own design
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
