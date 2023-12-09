#include "glrenderer.h"

void insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void makeTile(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight, std::vector<float> &m_vertexData) {


    glm::vec3 top_left_normal = glm::normalize(glm::cross(topLeft-bottomLeft,topLeft-bottomRight));
    glm::vec3 bottom_left_normal = glm::normalize(glm::cross(bottomLeft-bottomRight,bottomLeft-topLeft));
    glm::vec3 bottom_right_normal = glm::normalize(glm::cross(bottomRight-topRight,bottomRight-bottomLeft));
    glm::vec3 top_right_normal = glm::normalize(glm::cross(topRight-bottomLeft,topLeft-bottomLeft));

    glm::vec2 top_left_uv = glm::vec2(0,1);
    glm::vec2 bottom_left_uv = glm::vec2(0,0);
    glm::vec2 top_right_uv = glm::vec2(1,1);
    glm::vec2 bottom_right_uv = glm::vec2(1,0);


    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);

    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,bottom_left_normal);
    insertVec2(m_vertexData,bottom_left_uv);

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData,top_right_uv);

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);

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



std::vector<float> GLRenderer::generateRoadData(){
    std::vector<float> data;
    //Face on Y == 0., but facing up
    makeFace(glm::vec3(-0.5f, 0.f,-0.5f),
             glm::vec3( 0.5f, 0.f,-0.5f),
             glm::vec3(-0.5f, 0.f, 0.5f),
             glm::vec3(0.5f, 0.f, 0.5f),
             data);

    return data;
}




std::vector<float> GLRenderer::generateCubeData()
{
    std::vector<float> data;

    data.clear();
    //Face on Z = 1.
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             data);

    //Face on Y == 1.
    makeFace(glm::vec3(-0.5f, 0.5f,-0.5f),
             glm::vec3( 0.5f, 0.5f,-0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f),
             data);
    //Face on X == 1.
    makeFace(glm::vec3( 0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f,-0.5f),
             glm::vec3( 0.5f,-0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             data);

    //Face on Z = 0.
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             data);

    //Face on Y == 0.
    makeFace(glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f),
             data);

    //Face on X == 0.
    makeFace(glm::vec3(-0.5f, 0.5f,-0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f,-0.5f,-0.5f),
             glm::vec3(-0.5f,-0.5f, 0.5f),
             data);


    return data;
}

