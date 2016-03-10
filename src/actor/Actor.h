#if !defined(ACTOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define ACTOR_H
#include <string>
#include <algorithm>
#include <stdlib.h>
#include "../mesh/AABB.h"
#include "../mesh/Model.h"
#include "../player/Player.h"

class Player;

class Actor
{
public :
    Actor();
    Actor( glm::vec3 position );
    Actor( Model* model,glm::mat4 modelMatrix, std::string type );
    Actor( glm::vec3 position, std::string modelPath);

    glm::mat4 getModelMatrix() { return(modelMatrix); }
    Model* getModel(){return(model);}
    void setPosition( glm::mat4 newModelMatrix ) { modelMatrix = newModelMatrix; computeAABB();}
    AABB& getAABB(){return(aabb);}
    std::string getType() {return type;}
    virtual void doCollision( Player* p, float deltaTime ) = 0;
    virtual void Draw(Shader* s) = 0;
private :
    glm::mat4 modelMatrix;
    Model* model;
    AABB aabb;
    std::string type;
    void computeAABB();

};
#endif
