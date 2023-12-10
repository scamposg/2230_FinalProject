#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include <QOpenGLWidget>
#include "glm/glm.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    ~GLRenderer();

protected:
    void initializeGL()                  override; // Called once at the start of the program
    void paintGL()                       override; // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes

    void mousePressEvent(QMouseEvent *e) override; // Used for camera movement
    void mouseMoveEvent(QMouseEvent *e)  override; // Used for camera movement
    void wheelEvent(QWheelEvent *e)      override; // Used for camera movement
    void rebuildMatrices();                        // Used for camera movement

private:
    GLuint m_shader;     // Stores id of shader program
    GLuint m_cube_vbo; // Stores id of vbo
    GLuint m_cube_vao; // Stores id of vao
    std::vector<float> m_cubeData;

    std::vector<glm::mat4> m_matrices;
    glm::mat4 m_view_original;
    glm::mat4 m_view;
    glm::mat4 m_proj  = glm::mat4(1);
    glm::vec3 m_camera_pos = glm::vec3(0,20,10);
    glm::vec3 m_camera_look = glm::vec3(0,-1,-1);
    glm::vec3 m_camera_up = glm::vec3(0,1,0);

    glm::mat4 get_view_matrix();

    glm::vec4 m_lightPos; // The world-space position of the point light

    float m_ka;
    float m_kd;
    float m_ks;

    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;

    // UV Mapping stuff
    void loadTextures();
    QImage m_tex1;
    GLuint m_tex1_texture;
    QImage m_tex2;
    GLuint m_tex2_texture;
    QImage m_tex3;
    GLuint m_tex3_texture;
    QImage m_tex4;
    GLuint m_tex4_texture;
    QImage m_tex5;
    GLuint m_tex5_texture;
    QImage m_tex6;
    GLuint m_tex6_texture;
    QImage m_tex7;
    GLuint m_tex7_texture;
    QImage m_tex8;
    GLuint m_tex8_texture;
    QImage m_tex9;
    GLuint m_tex9_texture;
    QImage m_tex10;
    GLuint m_tex10_texture;
    QImage m_tex11;
    GLuint m_tex11_texture;
    QImage *texArray[11] = {&m_tex1, &m_tex2, &m_tex3, &m_tex4, &m_tex5, &m_tex6, &m_tex7, &m_tex8, &m_tex9, &m_tex10, &m_tex11};
    GLuint *textureArray[11] = {&m_tex1_texture, &m_tex2_texture, &m_tex3_texture, &m_tex4_texture, &m_tex5_texture, &m_tex6_texture,
                               &m_tex7_texture, &m_tex8_texture, &m_tex9_texture, &m_tex10_texture, &m_tex11_texture};
};
