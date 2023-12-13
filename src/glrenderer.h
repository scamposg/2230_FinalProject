#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include "glm/glm.hpp"
#include <unordered_map>
#include <QCoreApplication>
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QTime>
#include <QMouseEvent>
#include <QKeyEvent>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/constants.hpp"


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    bool m_camera_jitter=false;
    void play_scene();
    void reset_scene();
    void generate_city();
    void finish();
    ~GLRenderer();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL()                  override; // Called once at the start of the program
    void paintGL()                       override; // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes
    void timerEvent(QTimerEvent *event)  override;

    void rebuildMatrices();                        // Used for camera movement
    void paint_roads();
    void paint_buildings();

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override; // Used for camera movement
    void mouseMoveEvent(QMouseEvent *e)  override; // Used for camera movement
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *e)      override; // Used for camera movement

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames
    bool m_to_play = false;
    int m_device_pixel_ratio;
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not
    Qt::Key pressed_key;
    bool is_key_pressed;
    bool m_mouseDown;

    GLuint m_shader;     // Stores id of shader program
    GLuint m_shadow_shader;
    GLuint m_shadow_framebuffer;
    GLuint m_shadow_depth_texture;

    GLuint m_cube_vbo; // Stores id of vbo
    GLuint m_cube_vao; // Stores id of vao
    std::vector<float> m_cubeData;
    std::vector<glm::mat4> m_building_matrices;
    std::vector<glm::mat4> m_original_building_matrices;
    std::vector<float>m_building_z_buffer;
    QImage m_building_image;
    GLuint m_building_texture;
    std::vector<float> generateCubeData();

    GLuint m_road_vbo; // Stores id of vbo
    GLuint m_road_vao; // Stores id of vao
    std::vector<float> m_roadData;
    std::vector<glm::mat4> m_road_matrices;
    std::vector<glm::mat4> m_original_road_matrices;
    std::vector<float>m_road_z_buffer;
    QImage m_road_image;
    GLuint m_road_texture;
    QImage m_road_normal_image;
    GLuint m_road_normal_texture;
    std::vector<float> generateRoadData();


    void get_city_matrices();

    float m_radius = 50.f;
    std::vector<int> m_city_map; //x by z map where bottom left = index 0
    glm::vec3 building_ambient = glm::vec3(0.969,0.925,0.698);
    glm::vec3 building_diffuse = glm::vec3(1.0,0.965,0.784);
    glm::vec3 building_specular = glm::vec3(1.0,0.98,0.875);
    float closest_z=0.f;
    float furthest_z=0.f;




    glm::vec3 m_curve_OG_0 = glm::vec3(0,0,0);
    glm::vec3 m_curve_OG_1 = glm::vec3(0,0,-m_radius/3.f);
    glm::vec3 m_curve_OG_2 = glm::vec3(0,0,-m_radius*2.f/3.f);
    glm::vec3 m_curve_OG_3 = glm::vec3(0,0,-m_radius);


    glm::vec3 m_curve_0 = glm::vec3(0,0,0);
    glm::vec3 m_curve_1 = glm::vec3(0,0,-m_radius/3.f);
    glm::vec3 m_curve_2 = glm::vec3(0,0,-m_radius*2.f/3.f);
    glm::vec3 m_curve_3 = glm::vec3(0,0,-m_radius);

    glm::mat4 m_view_original;
    glm::mat4 m_view;
    glm::mat4 m_proj;

    float m_camera_jitter_value=0.1;

    glm::vec3 m_camera_pos = glm::vec3(0,5,10);
    glm::vec3 m_camera_look = glm::vec3(0,0,-1);
    glm::vec3 m_camera_up = glm::vec3(0,1,0);
    double m_near_plane = 0.1f;
    double m_far_plane = 200.f;
    double m_camera_width = glm::radians(45.0);
    double m_camera_height = glm::radians(45.0);

    void apply_bezier_matrices(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);


    glm::mat4 get_view_matrix();
    glm::mat4 get_proj_matrix();
    void rotate_camera(float theta, glm::vec3 axis);
    void translate_camera(float scale);
    bool key_pressed();
    bool key_released();

    glm::vec4 m_light_direction; // The world-space position of the point light

    float m_ka;
    float m_kd;
    float m_ks;
    float m_kt; // UV mapping for texture component
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;

    // UV Mapping and Normal Mapping stuff
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
    QImage m_norm1;
    GLuint m_norm1_texture;
    QImage m_norm2;
    GLuint m_norm2_texture;
    QImage m_norm3;
    GLuint m_norm3_texture;
    QImage m_norm4;
    GLuint m_norm4_texture;
    QImage m_norm5;
    GLuint m_norm5_texture;
    QImage m_norm6;
    GLuint m_norm6_texture;
    QImage m_norm7;
    GLuint m_norm7_texture;
    QImage m_norm8;
    GLuint m_norm8_texture;
    QImage m_norm9;
    GLuint m_norm9_texture;
    QImage m_norm10;
    GLuint m_norm10_texture;
    QImage m_norm11;
    GLuint m_norm11_texture;
    QImage *normArray[11] = {&m_norm1, &m_norm2, &m_norm3, &m_norm4, &m_norm5, &m_norm6, &m_norm7, &m_norm8, &m_norm9, &m_norm10, &m_norm11};
    GLuint *normTextureArray[11] = {&m_norm1_texture, &m_norm2_texture, &m_norm3_texture, &m_norm4_texture, &m_norm5_texture, &m_norm6_texture,
                                    &m_norm7_texture, &m_norm8_texture, &m_norm9_texture, &m_norm10_texture, &m_norm11_texture};
    QImage *texArray[11] = {&m_tex1, &m_tex2, &m_tex3, &m_tex4, &m_tex5, &m_tex6, &m_tex7, &m_tex8, &m_tex9, &m_tex10, &m_tex11};
    GLuint *textureArray[11] = {&m_tex1_texture, &m_tex2_texture, &m_tex3_texture, &m_tex4_texture, &m_tex5_texture, &m_tex6_texture,
                                &m_tex7_texture, &m_tex8_texture, &m_tex9_texture, &m_tex10_texture, &m_tex11_texture};
};
