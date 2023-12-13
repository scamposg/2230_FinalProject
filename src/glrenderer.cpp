#include "glrenderer.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include "src/shaderloader.h"
#include <unistd.h>
//#include <Windows.h>


GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_light_direction(10,-10,0,1),
      m_ka(0.1),
      m_kd(0.1),
      m_ks(0.5),
      m_shininess(15),
      m_angleX(6),
      m_angleY(0),
      m_zoom(2)
{
    rebuildMatrices();
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;
}

void GLRenderer::finish(){
    glDeleteBuffers(1,&m_cube_vbo);
    glDeleteVertexArrays(1,&m_cube_vao);
    glDeleteTextures(1,&m_building_texture);
    glDeleteBuffers(1,&m_road_vbo);
    glDeleteVertexArrays(1,&m_road_vao);
    glDeleteTextures(1,&m_road_texture);
    glDeleteProgram(m_shader);
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
    m_device_pixel_ratio = this->devicePixelRatio();

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
    m_shadow_shader = ShaderLoader::createShaderProgram(":/resources/shaders/shadow.vert",":/resources/shaders/shadow.frag");

    // Create the framebuffer
    glGenFramebuffers(1, &m_shadow_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_framebuffer);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &m_shadow_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_shadow_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, size().width() * m_device_pixel_ratio,
                 size().height() * m_device_pixel_ratio, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadow_depth_texture, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D,0);

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
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // UV Mapping
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    // Tangent vector for normal mapping
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(8 * sizeof(GLfloat)));

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
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // UV Mapping
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    // Tangent vector for normal mapping
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,11 * sizeof(GLfloat),reinterpret_cast<void *>(8 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    loadTextures();

    rebuildMatrices();
}

void GLRenderer::paintGL()
{
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paint_roads();
    paint_buildings();
    paint_grass();

}

void GLRenderer::paint_buildings(){

    srand(12345);
    for (int i=0; i < m_building_matrices.size(); i++) {
        // Bind Sphere Vertex Data
        glBindVertexArray(m_cube_vao);

        // Task 2: activate the shader program by calling glUseProgram with `m_shader`
        glUseProgram(m_shader);

        // Task 6: pass in m_model as a uniform into the shader program
        glm::mat4 model_matrix_glm = glm::mat4(*m_building_matrices[i]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model_matrix"),1,GL_FALSE,&model_matrix_glm[0][0]);

        glm::mat3 inverse_transpose = glm::inverse(glm::transpose(*m_building_matrices[i]));
        glUniformMatrix3fv(glGetUniformLocation(m_shader,"inverse_transpose_matrix"),1,GL_FALSE,&inverse_transpose[0][0]);

        // Task 7: pass in m_view and m_proj
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"view_matrix"),1,GL_FALSE,&m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"projection_matrix"),1,GL_FALSE,&m_proj[0][0]);

        // Task 12: pass m_ka into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_a"),m_ka);

        // Task 13: pass light position and m_kd into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_d"),m_kd);
        glUniform3fv(glGetUniformLocation(m_shader,"world_space_light_position"),1,&m_light_direction[0]);

        // Task 14: pass shininess, m_ks, and world-space camera position
        glm::vec3 camera_world_space = glm::vec3(glm::inverse(m_view) * glm::vec4(0.f,0.f,0.f,1.f));
        glUniform1f(glGetUniformLocation(m_shader,"k_s"),m_ks);
        glUniform3fv(glGetUniformLocation(m_shader,"camera_position"),1,&camera_world_space[0]);
        glUniform1f(glGetUniformLocation(m_shader,"shininess"),m_shininess);

        // UV and Normal mapping
        int random = rand() % 11;
        glUniform1i(glGetUniformLocation(m_shader, "objectTexture"), 0);
        glUniform1i(glGetUniformLocation(m_shader, "normalMap"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *textureArray[random]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *normTextureArray[random]);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 3);
        // Unbind Vertex Array
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        // Task 3: deactivate the shader program by passing 0 into glUseProgram
        glUseProgram(0);
    }
}

void GLRenderer::paint_roads(){

    for (int i=0; i < m_road_matrices.size(); i++) {
        // Bind Road Vertex Data
        glBindVertexArray(m_road_vao);

        // Activate the shader program by calling glUseProgram with `m_shader`
        glUseProgram(m_shader);


        // pass in m_model as a uniform into the shader program
        glm::mat4 model_matrix_glm = glm::mat4(*m_road_matrices[i]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model_matrix"),1,GL_FALSE,&model_matrix_glm[0][0]);

        glm::mat3 inverse_transpose = glm::inverse(glm::transpose(*m_road_matrices[i]));
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

        // UV and Normal mapping
        int random = rand() % 11;
        glUniform1i(glGetUniformLocation(m_shader, "objectTexture"), 0);
        glUniform1i(glGetUniformLocation(m_shader, "normalMap"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_road_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_road_normal_texture);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_roadData.size() / 3);
        // Unbind Vertex Array
        glBindTexture(GL_TEXTURE_2D,0);
        glBindVertexArray(0);

        // Task 3: deactivate the shader program by passing 0 into glUseProgram
        glUseProgram(0);
    }
}

void GLRenderer::paint_grass(){

    for (int i=0; i < m_grass_matrices.size(); i++) {
        // Bind Road Vertex Data
        glBindVertexArray(m_road_vao);

        // Activate the shader program by calling glUseProgram with `m_shader`
        glUseProgram(m_shader);


        // pass in m_model as a uniform into the shader program
        glm::mat4 model_matrix_glm = glm::mat4(*m_grass_matrices[i]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model_matrix"),1,GL_FALSE,&model_matrix_glm[0][0]);

        glm::mat3 inverse_transpose = glm::inverse(glm::transpose(*m_grass_matrices[i]));
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

        // UV and Normal mapping
        int random = rand() % 11;
        glUniform1i(glGetUniformLocation(m_shader, "objectTexture"), 0);
        glUniform1i(glGetUniformLocation(m_shader, "normalMap"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_grass_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_grass_normal_texture);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_roadData.size() / 3);
        // Unbind Vertex Array
        glBindTexture(GL_TEXTURE_2D,0);
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

void GLRenderer::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
    pressed_key = Qt::Key(event->key());
    is_key_pressed = true;

}

void GLRenderer::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
    is_key_pressed = false;
}

void GLRenderer::mousePressEvent(QMouseEvent *event) {
    // Set initial mouse position
    m_prevMousePos = event->pos();
    m_mouseDown = true;
}

void GLRenderer::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
    // Update angle member variables based on event parameters
    if (m_mouseDown){
        m_angleX = (event->position().x() - m_prevMousePos.x())*M_PI / ((float) width()*4.f);
        m_angleY = (event->position().y() - m_prevMousePos.y())*M_PI / ((float) height()*4.f);
        m_prevMousePos = event->pos();
        rotate_camera(m_angleX,glm::vec3(0,1,0));
        rotate_camera(m_angleY,glm::cross(m_camera_look,m_camera_up));
        rebuildMatrices();
    }
}

void GLRenderer::wheelEvent(QWheelEvent *event) {
    // Update zoom based on event parameter
//    m_zoom = event->angleDelta().y() / 100.f;
//    m_camera_pos -= (m_zoom*m_camera_pos);
    rebuildMatrices();
}

void GLRenderer::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f ;

    if (is_key_pressed){
        translate_camera(deltaTime * 10.f);
    }

    // Use deltaTime and m_keyMap here to move around
    if (m_to_play){
        m_camera_jitter_value = m_camera_jitter_value*-1.f;
        if (m_camera_jitter){
            m_camera_pos = m_camera_pos+m_camera_jitter_value*glm::vec3(1,0,0)/deltaTime;
            rebuildMatrices();
        }

        m_curve_1 = m_curve_OG_1+glm::vec3(0,std::min(1.f/deltaTime,0.3f),std::min(1.f/deltaTime,0.2f));
        m_curve_2 = m_curve_OG_2+glm::vec3(0,std::min((deltaTime+1.f)/deltaTime,0.5f),std::min((deltaTime+1.f)/deltaTime,0.5f));
        float z = -(m_radius-deltaTime-0.4f);
        float y = 4.f*sqrtf(z+m_radius);
        m_curve_3 = glm::vec3(0,y,z);
        apply_bezier_matrices(m_curve_0,m_curve_1,m_curve_2,m_curve_3);
    }
    else {
        m_elapsedTimer.restart();
    }

    update(); // asks for a PaintGL() call to occur
}

void GLRenderer::loadTextures() {
    for (int i = 0; i < 11; i++) {
        QString tex_filepath = QString((":/resources/textures/facade_diffuse" + std::to_string(i + 1) + ".png").c_str());
        QString norm_filepath = QString((":/resources/textures/facade_normal" + std::to_string(i + 1) + ".png").c_str());
        QImage *m_tex;
        GLuint *m_tex_texture;
        QImage *m_norm;
        GLuint *m_norm_texture;
        m_tex = texArray[i];
        m_tex_texture = textureArray[i];
        m_norm = normArray[i];
        m_norm_texture = normTextureArray[i];
        *m_tex = QImage(tex_filepath);
        *m_tex = m_tex->convertToFormat(QImage::Format_RGBA8888).mirrored();
        *m_norm = QImage(norm_filepath);
        *m_norm = m_norm->convertToFormat(QImage::Format_RGBA8888).mirrored();

        glGenTextures(1, &*m_tex_texture);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, *m_tex_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_tex->width(), m_tex->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex->bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind

        glGenTextures(1, &*m_norm_texture);
        glActiveTexture(1);
        glBindTexture(GL_TEXTURE_2D, *m_norm_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_norm->width(), m_norm->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_norm->bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind
    }

    QString road_filepath = QString(":/resources/textures/road_diffuse.jpeg");
    m_road_image = QImage(road_filepath);
    m_road_image = m_road_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1,&m_road_texture);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D,m_road_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_road_image.width(),m_road_image.height(),
                 0,GL_RGBA,GL_UNSIGNED_BYTE,m_road_image.bits());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    QString road_normal_filepath = QString(":/resources/textures/road_normal.png");
    m_road_normal_image = QImage(road_normal_filepath);
    m_road_normal_image = m_road_normal_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1,&m_road_normal_texture);
    glActiveTexture(1);
    glBindTexture(GL_TEXTURE_2D,m_road_normal_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_road_normal_image.width(),m_road_normal_image.height(),
                 0,GL_RGBA,GL_UNSIGNED_BYTE,m_road_normal_image.bits());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    QString grass_filepath = QString(":/resources/textures/grass_diffuse.png");
    m_grass_image = QImage(grass_filepath);
    m_grass_image = m_grass_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1,&m_grass_texture);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D,m_grass_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_grass_image.width(),m_grass_image.height(),
                 0,GL_RGBA,GL_UNSIGNED_BYTE,m_grass_image.bits());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    QString grass_normal_filepath = QString(":/resources/textures/grass_normal.png");
    m_grass_normal_image = QImage(grass_normal_filepath);
    m_grass_normal_image = m_grass_normal_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1,&m_grass_normal_texture);
    glActiveTexture(1);
    glBindTexture(GL_TEXTURE_2D,m_grass_normal_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_grass_normal_image.width(),m_grass_normal_image.height(),
                 0,GL_RGBA,GL_UNSIGNED_BYTE,m_grass_normal_image.bits());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
}
