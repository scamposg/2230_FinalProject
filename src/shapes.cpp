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

glm::vec3 tangentVector(std::vector<glm::vec3> &vertices, glm::vec3 &normal, std::vector<glm::vec2> &uv) {
    glm::vec3 edge1 = vertices[1] - vertices[0];
    glm::vec3 edge2 = vertices[2] - vertices[0];
    float deltaU1 = uv[1].x - uv[0].x;
    float deltaV1 = uv[1].y - uv[0].y;
    float deltaU2 = uv[2].x - uv[0].x;
    float deltaV2 = uv[2].y - uv[0].y;
    float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
    glm::vec3 tangent;
    tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
    tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
    tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);
    tangent = glm::normalize(tangent);
    return tangent;
}

void makeFacade(glm::vec3 topLeft,
                glm::vec3 topRight, glm::vec3 topMiddle,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight, std::vector<float> &m_vertexData){
    glm::vec3 top_left_normal = glm::normalize(glm::cross(topLeft-bottomLeft,topLeft-bottomRight));
    glm::vec3 bottom_left_normal = glm::normalize(glm::cross(bottomLeft-bottomRight,bottomLeft-topLeft));
    glm::vec3 bottom_right_normal = glm::normalize(glm::cross(bottomRight-topRight,bottomRight-bottomLeft));
    glm::vec3 top_right_normal = glm::normalize(glm::cross(topRight-bottomLeft,topLeft-bottomLeft));
    glm::vec3 top_middle_normal = glm::normalize(glm::cross(topMiddle-topLeft,topMiddle-topRight));

    glm::vec2 top_left_uv = glm::vec2(0,0.8);
    glm::vec2 bottom_left_uv = glm::vec2(0,0);
    glm::vec2 top_right_uv = glm::vec2(1,0.8);
    glm::vec2 bottom_right_uv = glm::vec2(1,0);
    glm::vec2 top_middle_uv = glm::vec2(0.5,1);

    std::vector<glm::vec3> vertices1 = {topLeft, bottomLeft, bottomRight};
    std::vector<glm::vec3> vertices2 = {bottomRight, topRight, topLeft};
    std::vector<glm::vec3> vertices3 = {topMiddle, topLeft, topRight};
    std::vector<glm::vec2> uv1 = {top_left_uv, bottom_left_uv, bottom_right_uv};
    std::vector<glm::vec2> uv2 = {bottom_right_uv, top_right_uv, top_left_uv};
    std::vector<glm::vec2> uv3 = {top_middle_uv, top_left_uv, top_right_uv};

    insertVec3(m_vertexData,topMiddle);
    insertVec3(m_vertexData,top_middle_normal);
    insertVec2(m_vertexData,top_middle_uv);
    insertVec3(m_vertexData, tangentVector(vertices3, top_middle_normal, uv3));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices3, top_left_normal, uv3));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData,top_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices3, top_right_normal, uv3));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, top_left_normal, uv1));

    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,bottom_left_normal);
    insertVec2(m_vertexData,bottom_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_left_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_right_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, bottom_right_normal, uv2));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData,top_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_right_normal, uv2));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_left_normal, uv2));
}

void makeRoof(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight, std::vector<float> &m_vertexData){
    glm::vec3 top_left_normal = glm::normalize(glm::cross(topLeft-bottomLeft,topLeft-bottomRight));
    glm::vec3 bottom_left_normal = glm::normalize(glm::cross(bottomLeft-bottomRight,bottomLeft-topLeft));
    glm::vec3 bottom_right_normal = glm::normalize(glm::cross(bottomRight-topRight,bottomRight-bottomLeft));
    glm::vec3 top_right_normal = glm::normalize(glm::cross(topRight-bottomLeft,topLeft-bottomLeft));

    glm::vec2 top_left_uv = glm::vec2(0,1);
    glm::vec2 bottom_left_uv = glm::vec2(0,0.8);
    glm::vec2 top_right_uv = glm::vec2(1,1);
    glm::vec2 bottom_right_uv = glm::vec2(1,0.8);

    std::vector<glm::vec3> vertices1 = {topLeft, bottomLeft, bottomRight};
    std::vector<glm::vec3> vertices2 = {bottomRight, topRight, topLeft};
    std::vector<glm::vec2> uv1 = {top_left_uv, bottom_left_uv, bottom_right_uv};
    std::vector<glm::vec2> uv2 = {bottom_right_uv, top_right_uv, top_left_uv};


    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, top_left_normal, uv1)); // Tangent vector for normal mapping

    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,bottom_left_normal);
    insertVec2(m_vertexData,bottom_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_left_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_right_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData,bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, bottom_right_normal, uv2));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData,top_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_right_normal, uv2));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData,top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_left_normal, uv2));
}

void makeTile(glm::vec3 topLeft,
              glm::vec3 topRight,
              glm::vec3 bottomLeft,
              glm::vec3 bottomRight, std::vector<float> &m_vertexData) {


    glm::vec3 top_left_normal = glm::normalize(glm::cross(topLeft-bottomLeft,topLeft-bottomRight));
    glm::vec3 bottom_left_normal = glm::normalize(glm::cross(bottomLeft-bottomRight,bottomLeft-topLeft));
    glm::vec3 bottom_right_normal = glm::normalize(glm::cross(bottomRight-topRight,bottomRight-bottomLeft));
    glm::vec3 top_right_normal = glm::normalize(glm::cross(topRight-bottomLeft,topLeft-bottomLeft));

    glm::vec2 top_left_uv = glm::vec2(0.0f, 1.0f);
    glm::vec2 bottom_left_uv = glm::vec2(0.0f, 0.0f);
    glm::vec2 bottom_right_uv = glm::vec2(1.0f, 0.0f);
    glm::vec2 top_right_uv = glm::vec2(1.0f, 1.0f);

    std::vector<glm::vec3> vertices1 = {topLeft, bottomLeft, bottomRight};
    std::vector<glm::vec3> vertices2 = {bottomRight, topRight, topLeft};
    std::vector<glm::vec2> uv1 = {top_left_uv, bottom_left_uv, bottom_right_uv};
    std::vector<glm::vec2> uv2 = {bottom_right_uv, top_right_uv, top_left_uv};

    insertVec3(m_vertexData,topLeft); // Coordinates
    insertVec3(m_vertexData,top_left_normal); // Normals
    insertVec2(m_vertexData, top_left_uv); // UV Coordinates
    insertVec3(m_vertexData, tangentVector(vertices1, top_left_normal, uv1)); // Tangent vector for normal mapping

    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,bottom_left_normal);
    insertVec2(m_vertexData, bottom_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_left_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData, bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices1, bottom_right_normal, uv1));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,bottom_right_normal);
    insertVec2(m_vertexData, bottom_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, bottom_right_normal, uv2));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,top_right_normal);
    insertVec2(m_vertexData, top_right_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_right_normal, uv2));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,top_left_normal);
    insertVec2(m_vertexData, top_left_uv);
    insertVec3(m_vertexData, tangentVector(vertices2, top_left_normal, uv2));

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
    //Facade on Z = 0.5
    makeFacade(glm::vec3(-0.5f,  0.4f, 0.5f),
             glm::vec3( 0.5f,  0.4f, 0.5f),
               glm::vec3(0.f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             data);

    //Left Roof
    makeRoof(glm::vec3( 0.f, 0.5f,-0.5f),
             glm::vec3( 0.f, 0.5f, 0.5f),
             glm::vec3(-0.5f, 0.4f, -0.5f),
             glm::vec3(-0.5f, 0.4f, 0.5f),
             data);

    //Right Roof
    makeRoof(glm::vec3( 0.f, 0.5f, 0.5f),
             glm::vec3( 0.f, 0.5f,-0.5f),
             glm::vec3(0.5f, 0.4f, 0.5f),
             glm::vec3(0.5f, 0.4f,-0.5f),
             data);

    //Face on X == 0.5
    makeFace(glm::vec3( 0.5f, 0.4f, 0.5f),
             glm::vec3( 0.5f, 0.4f,-0.5f),
             glm::vec3( 0.5f,-0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f,-0.5f),
             data);

    //Facade on Z = 0.5
    makeFacade(glm::vec3( 0.5f,  0.4f,-0.5f),
               glm::vec3(-0.5f,  0.4f,-0.5f),
               glm::vec3( 0.f, 0.5f,-0.5f),
               glm::vec3( 0.5f, -0.5f,-0.5f),
               glm::vec3(-0.5f, -0.5f,-0.5f),
               data);

    //Face on Y == -0.5
//    makeFace(glm::vec3( 0.5f, -0.5f, 0.5f),
//             glm::vec3( 0.5f, -0.5f,-0.5f),
//             glm::vec3(-0.5f, -0.5f, 0.5f),
//             glm::vec3(-0.5f, -0.5f,-0.5f),
//             data);

    //Face on X == -0.5
    makeFace(glm::vec3(-0.5f, 0.4f,-0.5f),
             glm::vec3(-0.5f, 0.4f, 0.5f),
             glm::vec3(-0.5f,-0.5f,-0.5f),
             glm::vec3(-0.5f,-0.5f, 0.5f),
             data);


    return data;
}

