#if !defined(PLAYER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */

#define PLAYER_H

#include "../mesh/Model.h"
#include "../camera/Camera.h"
#include "../controller/Controller.h"
#include <map>
#include <utility>
#include "glm/glm.hpp"
#include "../managers/ControllerManager.h"
#include <iostream>

class Player
{
public:
    Player(int forwardKey = GLFW_KEY_Z,
           int backwardKey = GLFW_KEY_S,
           int rightKey = GLFW_KEY_Q,
           int leftKey = GLFW_KEY_D);
    ~Player();

    
    void mouseMoveCallback( int x, int y );
    void startMoveForward();
    void startMoveBackward();
    void startMoveRight();
    void startMoveLeft();
    void stopMoveForward();
    void stopMoveBackward();
    void stopMoveRight();
    void stopMoveLeft();
    void stopMoveUp();;
    void startMoveUp();
    void startMoveDown();
    void stopMoveDown();
	void canMoveTrue();
	void canMoveFalse();

    void move(float deltaTime);
    void updateCamera();
    void mouseMotion(int x, int y);
    
    std::map<int, std::string> getMapping(){return(mapping);}
    Controller* getController(){return(c);}
    Camera* getCamera(){return(cam);}
    void setPosition( glm::vec3 p ) { position = p; updateCamera();}
    glm::vec3 getPosition(){return(position);}

    bool getMoveLeft() { return(moveLeft);}
    bool getMoveRight() { return(moveRight); }
    bool getMoveForward() { return(moveForward); }
    bool getMoveBackward() { return(moveBackward); }

private:
    int forwardKey;
    int backwardKey;
    int rightKey;
    int leftKey;

    bool moveLeft;
    bool moveRight;
    bool moveForward;
    bool moveBackward;
    bool moveUp;
    bool moveDown;
	bool canMove;
    bool firstTime;
    
    std::map<int, std::string> mapping;
    float speed;
    glm::vec3 position;
    Controller* c;
    Camera* cam;

    float lastX;
    float lastY;
    bool firstCursorMov;
};

#endif
