#include "glrenderer.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include <QMouseEvent>
#include <unistd.h>
//#include <Windows.h>


GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_light_direction(0.1,-100,-10,0),
      m_ka(0.1),
      m_kd(0.2),
      m_ks(1),
      m_kt(0.8), // Might want to change these values
      m_shininess(15),
      m_angleX(6),
      m_angleY(0),
      m_zoom(2)
{
    rebuildMatrices();
}

GLRenderer::~GLRenderer()
{
    makeCurrent();
    doneCurrent();
}

// ================== Helper Functions



// ================== Students, You'll Be Working In These Files

void GLRenderer::initializeGL()
{
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initialize GL extension wrangler
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set clear color to black
    glClearColor(0.75,0.9,0.95,1);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Task 1: call ShaderLoader::createShaderProgram with the paths to the vertex
    //         and fragment shaders. Then, store its return value in `m_shader`
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",":/resources/shaders/default.frag");

    // Generate and bind VBO
    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    // Generate sphere data
    m_cubeData = generateCubeData();
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER,m_cubeData.size() * sizeof(GLfloat),m_cubeData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // UV Mapping
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    // Generate and bind VBO
    glGenBuffers(1, &m_road_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_road_vbo);
    // Generate sphere data
    m_roadData = generateRoadData();
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER,m_roadData.size() * sizeof(GLfloat),m_roadData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_road_vao);
    glBindVertexArray(m_road_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // UV Mapping
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    m_view_original = get_view_matrix();
}

void GLRenderer::paintGL()
{
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i=0; i < m_building_matrices.size(); i++) {
        // Bind Sphere Vertex Data
        glBindVertexArray(m_road_vao);

        // Activate the shader program by calling glUseProgram with `m_shader`
        glUseProgram(m_shader);

        // Pass in building color data
        glUniform3fv(glGetUniformLocation(m_shader,"object_ambient"),1,&building_ambient[0]);
        glUniform3fv(glGetUniformLocation(m_shader,"object_diffuse"),1,&building_diffuse[0]);
        glUniform3fv(glGetUniformLocation(m_shader,"object_specular"),1,&building_specular[0]);

        // pass in m_model as a uniform into the shader program
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model_matrix"),1,GL_FALSE,&m_building_matrices[i][0][0]);
        glm::mat4 model_matrix_glm = glm::mat4(m_building_matrices[i]);
        glm::mat3 inverse_transpose = glm::inverse(glm::transpose(m_building_matrices[i]));
        glUniformMatrix3fv(glGetUniformLocation(m_shader,"inverse_transpose_matrix"),1,GL_FALSE,&inverse_transpose[0][0]);

        // pass in m_view and m_proj
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"view_matrix"),1,GL_FALSE,&m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"projection_matrix"),1,GL_FALSE,&m_proj[0][0]);


        // pass m_ka into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_a"),m_ka);

        // pass light position and m_kd into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_d"),m_kd);
        glUniform3fv(glGetUniformLocation(m_shader,"world_space_light_direction"),1,&m_light_direction[0]);

        // pass shininess, m_ks, and world-space camera position
        glm::vec3 camera_world_space = glm::vec3(glm::inverse(m_view) * glm::vec4(0.f,0.f,0.f,1.f));
        glUniform1f(glGetUniformLocation(m_shader,"k_s"),m_ks);
        glUniform3fv(glGetUniformLocation(m_shader,"camera_position"),1,&camera_world_space[0]);
        glUniform1f(glGetUniformLocation(m_shader,"shininess"),m_shininess);

        // UV mapping
        glUniform1f(glGetUniformLocation(m_shader,"k_t"),m_kt);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_roadData.size() / 3);
        // Unbind Vertex Array
        glBindVertexArray(0);

        // Task 3: deactivate the shader program by passing 0 into glUseProgram
        glUseProgram(0);
    }
}

// ================== Other stencil code

void GLRenderer::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::radians(45.0),1.0 * w / h,0.01,100.0);
}

void GLRenderer::mousePressEvent(QMouseEvent *event) {
    // Set initial mouse position
    m_prevMousePos = event->pos();
}

void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
    // Update angle member variables based on event parameters
    m_angleX = (event->position().x() - m_prevMousePos.x())*M_PI / ((float) width()*4.f);
    m_angleY = (event->position().y() - m_prevMousePos.y())*M_PI / ((float) height()*4.f);
    m_prevMousePos = event->pos();
    rotate_camera(m_angleX,glm::vec3(0,1,0));
    rotate_camera(m_angleY,glm::cross(m_camera_look,m_camera_up));
    rebuildMatrices();
}

void GLRenderer::wheelEvent(QWheelEvent *event) {
    // Update zoom based on event parameter
    m_zoom = event->angleDelta().y() / 100.f;
    m_camera_pos -= (m_zoom*m_camera_pos);
    rebuildMatrices();
}

void GLRenderer::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f ;

    // Use deltaTime and m_keyMap here to move around
    if (m_to_play){
//        m_curve_3 = m_radius*glm::vec3(0,sin(deltaTime*M_PI),-cos(deltaTime*M_PI));
        m_curve_1 = m_curve_OG_1+glm::vec3(0,0,deltaTime);
        m_curve_2 = m_curve_OG_2+glm::vec3(0,deltaTime,deltaTime);
        float z = -(m_radius-2.f*deltaTime);
//        float y = std::log(z+m_radius+1);
        float y = 5.f*sqrtf(z+m_radius)-0.5;
        m_curve_3 = glm::vec3(0,y,z);
        apply_bezier_matrices(m_curve_0,m_curve_1,m_curve_2,m_curve_3);
    }
    else {
        m_elapsedTimer.restart();
    }

    update(); // asks for a PaintGL() call to occur
}

void GLRenderer::rebuildMatrices() {
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = get_view_matrix();
    m_proj = get_proj_matrix();


    update();
}
