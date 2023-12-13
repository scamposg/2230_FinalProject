#include "glrenderer.h"

glm::mat4 get_translation_mat(glm::vec3 shift){
    glm::vec4 col0(1.f,0.f,0.f,0.f);
    glm::vec4 col1(0.f,1.f,0.f,0.f);
    glm::vec4 col2(0.f,0.f,1.f,0.f);
    glm::vec4 col3(-shift.x,-shift.y,-shift.z,1.f);

    return glm::mat4(col0,col1,col2,col3);
}

glm::mat3 get_rotation_mat(float theta, glm::vec3 u){
    glm::vec3 col0(std::cos(theta) + (std::pow(u.x,2.f)*(1-std::cos(theta))),
                   u.x*u.y*(1-std::cos(theta) + u.z*std::sin(theta)),
                   u.x*u.z*(1-std::cos(theta)) - u.y*std::sin(theta));

    glm::vec3 col1(u.x*u.y*(1-std::cos(theta)) - u.z*std::sin(theta),
                   std::cos(theta) + (std::pow(u.y,2.f)*(1-std::cos(theta))),
                   u.y*u.z*(1-std::cos(theta)) + u.x*std::sin(theta));

    glm::vec3 col2(u.x*u.z*(1-std::cos(theta)) + u.y*std::sin(theta),
                   u.y*u.z*(1-std::cos(theta)) - u.x*std::sin(theta),
                   std::cos(theta) + (std::pow(u.z,2.f)*(1-std::cos(theta))));

    return glm::mat3(col0,col1,col2);
}

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

glm::mat4 GLRenderer::get_proj_matrix() {

    double theta_w = m_camera_width;
    double theta_h = m_camera_height;
    double c = -m_near_plane/m_far_plane;
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



void GLRenderer::rotate_camera(float theta, glm::vec3 axis){
    glm::mat3 transform = get_rotation_mat(theta,axis);
    glm::vec3 old_look_vector = m_camera_look;
    glm::vec3 new_look_vector = transform*old_look_vector;
    m_camera_look = glm::vec4(new_look_vector,0.f);


}

bool GLRenderer::key_released(){
    if (m_keyMap[pressed_key] == false) return true;
    return false;
}

bool GLRenderer::key_pressed(){
    if (m_keyMap[Qt::Key_W]){
        pressed_key = Qt::Key_W;
        return true;
    }
    else if (m_keyMap[Qt::Key_S]){
        pressed_key = Qt::Key_S;
        return true;
    }
    else if (m_keyMap[Qt::Key_A]){
        pressed_key = Qt::Key_A;
        return true;
    }
    else if (m_keyMap[Qt::Key_D]){
        pressed_key = Qt::Key_D;
        return true;
    }
    else if (m_keyMap[Qt::Key_Space]){
        pressed_key = Qt::Key_Space;
        return true;
    }
    else if (m_keyMap[Qt::Key_Control]){
        pressed_key = Qt::Key_Control;
        return true;
    }

    return false;
}

void GLRenderer::translate_camera(float scale){
    glm::mat4 transform;
    glm::vec3 shift;
    switch(pressed_key){
    case(Qt::Key_W):
        //move in look vector
        shift = -scale*glm::normalize(glm::vec3(m_camera_look));
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    case(Qt::Key_S):
        //move in -look vector
        shift = scale*glm::normalize(glm::vec3(m_camera_look));
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    case(Qt::Key_A):
        //move left perpendicular to look & up vectors
        shift = scale*glm::normalize(glm::cross(glm::vec3(m_camera_look),
                                                  glm::vec3(m_camera_up)));
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    case(Qt::Key_D):
        //move right perpendicular to look & up vectors
        shift = scale*-glm::normalize(glm::cross(glm::vec3(m_camera_look),
                                                   glm::vec3(m_camera_up)));
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    case(Qt::Key_Space):
        //move to (0,1,0)
        shift = scale*glm::vec4(0.f,1.f,0.f,1.f);
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    case(Qt::Key_Control):
        //move to (0,-1,0)
        shift = scale*glm::vec4(0.f,-1.f,0.f,1.f);
        transform = get_translation_mat(shift);
        m_camera_pos = transform*glm::vec4(m_camera_pos,1.f);
        break;
    }

    rebuildMatrices();
}


void GLRenderer::rebuildMatrices() {
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = get_view_matrix();
    m_proj = get_proj_matrix();


    update();
}
