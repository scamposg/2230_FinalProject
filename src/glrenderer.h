#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include "glm/glm.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    void play_scene();
    ~GLRenderer();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL()                  override; // Called once at the start of the program
    void paintGL()                       override; // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes
    void timerEvent(QTimerEvent *event)  override;
    void mousePressEvent(QMouseEvent *e) override; // Used for camera movement
    void mouseMoveEvent(QMouseEvent *e)  override; // Used for camera movement
    void wheelEvent(QWheelEvent *e)      override; // Used for camera movement
    void rebuildMatrices();                        // Used for camera movement

private:

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames
    bool m_to_play = false;

    GLuint m_shader;     // Stores id of shader program
    GLuint m_cube_vbo; // Stores id of vbo
    GLuint m_cube_vao; // Stores id of vao
    std::vector<float> m_cubeData;

    std::vector<glm::mat4> m_matrices;
    glm::vec3 m_curve_0 = glm::vec3(0,0,20);
    glm::vec3 m_curve_1 = glm::vec3(0,0,-20);
    glm::vec3 m_curve_2 = glm::vec3(0,0,-30);

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
    float m_kt; // UV mapping for texture component
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;
};
