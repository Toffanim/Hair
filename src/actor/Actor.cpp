/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */
#include "Actor.h"
using namespace std;
//Main actor class from which every actor in the scene (chest, monster, wall, water) will inherits
Actor::Actor( Model* model, glm::mat4 modelMatrix, std::string type)
        : model(model), modelMatrix(modelMatrix), type(type)
{
    computeAABB();
}

//Compute AABB based on the loaded model and position
void Actor::computeAABB()
{
    
    aabb.min = glm::vec3( FLT_MAX, FLT_MAX, FLT_MAX);
    aabb.max = glm::vec3( FLT_MIN, FLT_MIN, FLT_MIN);
    //aabb.size = glm::vec3();
    //aabb.center = glm::vec3();
    vector<Mesh> m = model->getMeshes();
    for( vector<Mesh>::iterator it = m.begin();
         it != m.end();
         ++it)
    {
        vector<Vertex> v = (*it).vertices;
        for( vector<Vertex>::iterator it = v.begin();
             it != v.end();
             ++it)
        {
            glm::vec4 vertex = modelMatrix * glm::vec4( (*it).Position, 1.0f);
            if ( vertex.x > aabb.max.x ) { aabb.max.x = vertex.x; }
            if ( vertex.y > aabb.max.y ) { aabb.max.y = vertex.y; }
            if ( vertex.z > aabb.max.z ) { aabb.max.z = vertex.z; }
            if ( vertex.x < aabb.min.x ) { aabb.min.x = vertex.x; }
            if ( vertex.y < aabb.min.y ) { aabb.min.y = vertex.y; }
            if ( vertex.z < aabb.min.z ) { aabb.min.z = vertex.z; }
        }
    }    
    aabb.size = glm::vec3(aabb.max.x-aabb.min.x, aabb.max.y-aabb.min.y, aabb.max.z-aabb.min.z);
    aabb.center = glm::vec3((aabb.min.x+aabb.max.x)/2, (aabb.min.y+aabb.max.y)/2, (aabb.min.z+aabb.max.z)/2);
    //glm::mat4 transform =  glm::translate(glm::mat4(1), center)* glm::scale(glm::mat4(1), size);

}
