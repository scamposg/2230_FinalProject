#include "glrenderer.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_lightPos(10,0,0,1),
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



std::vector<glm::mat4> create_model_matrices(){
    std::vector<glm::mat4> matrices;
    glm::vec3 P0(0,0,20);
    glm::vec3 P1(0,0,-20);
    glm::vec3 P2(0,10,-10);

    matrices.clear();
    float interval = 2.f;
    float max = 10.f;
    for (float x = -max; x < max; ){
        for (float t = 0.0; t < 1.0; ){
            glm::vec3 curve = (1.f-t*t) * P0 + 2.f*(1.f-t)*t*P1 + t*t*P2;
            glm::vec3 ray = curve-glm::vec3(0,0,10);
            float theta = acos(glm::dot(ray,curve)/abs(glm::length(curve)*glm::length(ray)));
            glm::mat4 ctm = glm::translate(glm::vec3(x,curve.y,curve.z))*glm::rotate(theta,glm::vec3(0,1,0));
            matrices.push_back(ctm);
            t= t+0.1;
        }
        x = x + interval;
    }

    return matrices;
}

void insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, std::vector<float> &m_vertexData) {


    glm::vec3 top_left_normal = glm::normalize(glm::cross(topLeft-bottomLeft,topLeft-bottomRight));
    glm::vec3 bottom_left_normal = glm::normalize(glm::cross(bottomLeft-bottomRight,bottomLeft-topLeft));
    glm::vec3 bottom_right_normal = glm::normalize(glm::cross(bottomRight-topRight,bottomRight-bottomLeft));
    glm::vec3 top_right_normal = glm::normalize(glm::cross(topRight-bottomLeft,topLeft-bottomLeft));

    insertVec3(m_vertexData,topLeft); // Coordinates
    insertVec3(m_vertexData,top_left_normal); // Normals
    insertVec2(m_vertexData, glm::vec2(0.0f, 1.0f)); // UV Coordinates

    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,bottom_left_normal);
    insertVec2(m_vertexData, glm::vec2(0.0f, 0.0f));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData, glm::vec2(1.0f, 0.0f));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData, glm::vec2(1.0f, 0.0f));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData, glm::vec2(1.0f, 1.0f));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData, glm::vec2(0.0f, 1.0f));
}

void makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, std::vector<float> &m_vertexData) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube

    //default set everything to top and bottom  lefts
    int m_param1 = 1;

    for (float row_ctr=0.f; row_ctr<float(m_param1); row_ctr++){
        glm::vec3 tile_top_left = bottomLeft;
        glm::vec3 tile_top_right = bottomLeft;
        glm::vec3 tile_bottom_left = bottomLeft;
        glm::vec3 tile_bottom_right = bottomLeft;


        tile_top_left += ((row_ctr+1.f)/m_param1)*(topLeft-bottomLeft);
        tile_top_right += ((row_ctr+1.f)/m_param1)*(topLeft-bottomLeft);
        tile_bottom_left += ((row_ctr)/m_param1)*(topLeft-bottomLeft);
        tile_bottom_right += ((row_ctr)/m_param1)*(topLeft-bottomLeft);

        for (float col_ctr=0.f; col_ctr<float(m_param1); col_ctr++){

            //iterate through column values


            tile_top_right += (topRight-topLeft)/float(m_param1);
            tile_bottom_right += (topRight-topLeft)/float(m_param1);

            makeTile(tile_top_left,tile_top_right,tile_bottom_left,tile_bottom_right,m_vertexData);
            tile_top_left += (topRight-topLeft)/float(m_param1);
            tile_bottom_left += (topRight-topLeft)/float(m_param1);
        }
    }
}

std::vector<float> generateCubeData()
{
    std::vector<float> data;

    data.clear();
    //Face on Z = 0.5
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             data);

    //Face on Y == 0.5
    makeFace(glm::vec3(-0.5f, 0.5f,-0.5f),
             glm::vec3( 0.5f, 0.5f,-0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             data);
    //Face on X == 0.5
    makeFace(glm::vec3( 0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f,-0.5f),
             glm::vec3( 0.5f,-0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             data);

    //Face on Z = -0.5
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             data);

    //Face on Y == -0.5
    makeFace(glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f),
             data);

    //Face on X == -0.5
    makeFace(glm::vec3(-0.5f, 0.5f,-0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f,-0.5f,-0.5f),
             glm::vec3(-0.5f,-0.5f, 0.5f),
             data);


    return data;
}

// ================== Students, You'll Be Working In These Files

void GLRenderer::initializeGL()
{
    // Initialize GL extension wrangler
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set clear color to black
    glClearColor(0.5,0.5,0.5,1);

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
    loadTextures();

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    m_matrices = create_model_matrices();
    m_view_original = get_view_matrix();
}

void GLRenderer::paintGL()
{
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i=0; i < m_matrices.size(); i++) {
        // Bind Sphere Vertex Data
        glBindVertexArray(m_cube_vao);

        // Task 2: activate the shader program by calling glUseProgram with `m_shader`
        glUseProgram(m_shader);

        // Task 6: pass in m_model as a uniform into the shader program
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model_matrix"),1,GL_FALSE,&m_matrices[i][0][0]);
        glm::mat4 model_matrix_glm = glm::mat4(m_matrices[i]);
        glm::mat3 inverse_transpose = glm::inverse(glm::transpose(m_matrices[i]));
        glUniformMatrix3fv(glGetUniformLocation(m_shader,"inverse_transpose_matrix"),1,GL_FALSE,&inverse_transpose[0][0]);

        // Task 7: pass in m_view and m_proj
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"view_matrix"),1,GL_FALSE,&m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"projection_matrix"),1,GL_FALSE,&m_proj[0][0]);


        // Task 12: pass m_ka into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_a"),m_ka);

        // Task 13: pass light position and m_kd into the fragment shader as a uniform
        glUniform1f(glGetUniformLocation(m_shader,"k_d"),m_kd);
        glUniform3fv(glGetUniformLocation(m_shader,"world_space_light_position"),1,&m_lightPos[0]);

        // Task 14: pass shininess, m_ks, and world-space camera position
        glm::vec3 camera_world_space = glm::vec3(glm::inverse(m_view) * glm::vec4(0.f,0.f,0.f,1.f));
        glUniform1f(glGetUniformLocation(m_shader,"k_s"),m_ks);
        glUniform3fv(glGetUniformLocation(m_shader,"camera_position"),1,&camera_world_space[0]);
        glUniform1f(glGetUniformLocation(m_shader,"shininess"),m_shininess);

        // UV mapping
        glUniform1f(glGetUniformLocation(m_shader,"k_t"),m_kt);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 3);
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
    m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
    m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
    m_prevMousePos = event->pos();
    rebuildMatrices();
}

void GLRenderer::wheelEvent(QWheelEvent *event) {
    // Update zoom based on event parameter
    m_zoom -= event->angleDelta().y() / 100.f;
    rebuildMatrices();
}

void GLRenderer::rebuildMatrices() {
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = m_view_original;

    glm::mat4 rot = glm::rotate(glm::radians(10 * m_angleX),glm::vec3(0,0,1));
    glm::vec3 eye = m_camera_pos;
    eye = glm::vec3(rot * glm::vec4(eye,1));

    rot = glm::rotate(glm::radians(10 * m_angleY),glm::cross(m_camera_look,m_camera_up));
    eye = glm::vec3(rot * glm::vec4(eye,1));

    eye = eye * m_zoom;

    m_view = glm::lookAt(eye,glm::vec3(0,0,0),m_camera_up);

    m_proj = glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,100.0);

    update();
}

void GLRenderer::loadTextures() {
    QString tex1_filepath = QString(":/resources/textures/facade_diffuse1.png");

    // Task 1: Obtain image from filepath
    m_tex1 = QImage(tex1_filepath);
    // Task 2: Format image to fit OpenGL
    m_tex1 = m_tex1.convertToFormat(QImage::Format_RGBA8888).mirrored();
    // Task 3: Generate kitten texture
    glGenTextures(1, &m_tex1_texture);
    // Task 9: Set the active texture slot to texture slot 0
    glActiveTexture(GL_TEXTURE0);
    // Task 4: Bind kitten texture
    glBindTexture(GL_TEXTURE_2D, m_tex1_texture);
    // Task 5: Load image into kitten texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_tex1.width(), m_tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex1.bits());
    // Task 6: Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Task 7: Unbind kitten texture
    glBindTexture(GL_TEXTURE_2D, 0);
    // Task 10: Set the texture.frag uniform for our texture
    glUseProgram(m_shader);
    glUniform1i(glGetUniformLocation(m_shader, "objectTexture"), GL_TEXTURE0);
    glUseProgram(0);
}
