/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Player.h"
using namespace std;

Player::Player(int forwardKey, int backwardKey, int rightKey, int leftKey) :
        forwardKey(forwardKey), backwardKey(backwardKey), rightKey(rightKey), leftKey(leftKey),
		moveForward(false), moveRight(false), moveLeft(false), moveBackward(false), moveUp(false), moveDown(false), canMove(false),
        speed(2.0f), position( glm::vec3( 0.0f, 0.0f, 0.0f )),
        lastX(0.0f), lastY(0.0f), firstTime(true), life(100), gold(0)
        , atkPerSec( 0.5f ), atk( 2 ), lastAttack( 0.0f ), key(1)
{
    
    c = new Controller();
    ControllerManager& cm = ControllerManager::getInstance();
    cm.add(c);
    
    c->addKeyListener( pair<int, int>(forwardKey, GLFW_PRESS), bind(&Player::startMoveForward, this));
    c->addKeyListener( pair<int, int>(backwardKey, GLFW_PRESS), bind(&Player::startMoveBackward, this));
    c->addKeyListener( pair<int, int>(rightKey, GLFW_PRESS), bind(&Player::startMoveRight, this));
    c->addKeyListener( pair<int, int>(leftKey, GLFW_PRESS), bind(&Player::startMoveLeft, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_UP, GLFW_PRESS), bind(&Player::startMoveUp, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_DOWN, GLFW_PRESS), bind(&Player::startMoveDown, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_LEFT_ALT, GLFW_PRESS), bind(&Player::canMoveTrue, this));

    c->addKeyListener( pair<int, int>(forwardKey, GLFW_RELEASE), bind(&Player::stopMoveForward, this));
    c->addKeyListener( pair<int, int>(backwardKey, GLFW_RELEASE), bind(&Player::stopMoveBackward, this));
    c->addKeyListener( pair<int, int>(rightKey, GLFW_RELEASE), bind(&Player::stopMoveRight, this));
    c->addKeyListener( pair<int, int>(leftKey, GLFW_RELEASE), bind(&Player::stopMoveLeft, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_UP, GLFW_RELEASE), bind(&Player::stopMoveUp, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_DOWN, GLFW_RELEASE), bind(&Player::stopMoveDown, this));
	c->addKeyListener(pair<int, int>(GLFW_KEY_LEFT_ALT, GLFW_RELEASE), bind(&Player::canMoveFalse, this));

    std::function<void(double, double)> f = bind(&Player::mouseMotion, this, std::placeholders::_1, std::placeholders::_2);
    c->addCursorListener( f );
    
    #if 0
    c->setKeyPressCallback(forwardKey, bind(&Player::startMoveForward, this));
    c->setKeyPressCallback(backwardKey, bind(&Player::startMoveBackward, this));
    c->setKeyPressCallback(rightKey, bind(&Player::startMoveRight, this));
    c->setKeyPressCallback(leftKey, bind(&Player::startMoveLeft, this));
    c->setKeyReleaseCallback(forwardKey, bind(&Player::stopMoveForward, this));
    c->setKeyReleaseCallback(backwardKey, bind(&Player::stopMoveBackward, this));
    c->setKeyReleaseCallback(rightKey, bind(&Player::stopMoveRight, this));
    c->setKeyReleaseCallback(leftKey, bind(&Player::stopMoveLeft, this));
    c->setKeyPressCallback(SDLK_UP, bind(&Player::startMoveUp, this));
    c->setKeyReleaseCallback(SDLK_UP, bind(&Player::stopMoveUp, this));
    c->setKeyPressCallback(SDLK_DOWN, bind(&Player::startMoveDown, this));
    c->setKeyReleaseCallback(SDLK_DOWN, bind(&Player::stopMoveDown, this));
    std::function<void(int, int)> f = bind(&Player::mouseMotion, this, std::placeholders::_1, std::placeholders::_2);
    c->setMouseCallback(f);
    #endif
    
    cam = new Camera( position );

    aabb.min = glm::vec3();
    aabb.max = glm::vec3();
    aabb.size = glm::vec3();
    aabb.center = glm::vec3();
}

Player::~Player()
{
    if(c)
    { delete c; }
    if(cam)
    { delete cam; }
}

//Is the player currently having a key ?
bool Player::hasKey()
{
    return key > 0 ? true : false;
}

//Is the player attacking ?
bool Player::isAttacking( float deltaTime )
{
    lastAttack += deltaTime;
    if ( lastAttack >= atkPerSec)
    {
        lastAttack = 0.0f;
        return true;
    }return false;
}


//Pretty self explaining
//Pretty self explaining
void Player::canMoveTrue()
{
	canMove = true;
}
//Pretty self explaining
void Player::canMoveFalse()
{
	canMove = false;
}
void Player::startMoveUp()
{
    moveUp = true;
}
void Player::startMoveDown()
{
    moveDown = true;
}
void Player::stopMoveDown()
{
    moveDown=false;
}
void Player::stopMoveUp()
{
    moveUp = false;
}
void Player::stopMoveForward()
{
    moveForward = false;
}
void Player::stopMoveBackward()
{
    moveBackward = false;
}
void Player::stopMoveRight()
{
    moveRight = false;
}
void Player::stopMoveLeft()
{
    moveLeft = false;
}
void Player::startMoveForward()
{
    moveForward = true;
}
void Player::startMoveBackward()
{
    moveBackward = true;
}
void Player::startMoveRight()
{
    moveRight = true;
}
void Player::startMoveLeft()
{
    moveLeft = true;
}

//Apply movement based on the boolean seen ^^^^^ before ^^^^^
void Player::move(float deltaTime)
{

#if 0 
    glm::vec3 pos = position;
    float x = pos.x;
    float z = pos.y;
    float y = pos.z;

	float r = sqrt(x*x + y*y + z*z);
	float theta = acos(z / r);
	float phi;
	if (x > 0.0001)
		phi = atan(y / x);
	else
		phi = 0.f;

	
	glm::vec3 dir = glm::normalize(pos);
    if(moveForward)
    {
        r -= speed*deltaTime;
    }
    if(moveBackward)
    {
        r += speed*deltaTime;
    }
    if(moveRight)
    {
        phi += speed*deltaTime;
    }
    if(moveLeft)
    {
        phi -= speed*deltaTime;
    }
    if(moveUp)
    {
        theta += speed*deltaTime;
    }
    if(moveDown)
    {
        theta -= speed*deltaTime;
    }
    
	if (moveDown || moveUp || moveForward || moveBackward || moveRight || moveLeft)
	{
		x = r * cos(theta) * sin(phi);
		y = r * sin(theta) * sin(phi);
		z = r * cos(theta);
	}

    position = glm::vec3(x,z,y);
    computeAABB();
    updateCamera();
#endif

	glm::vec3 pos = position;
	float x = pos.x;
	float y = pos.y;
	float z = pos.z;

	glm::vec3 t = cam->getFront();
	t.y = 0.0;
	t = glm::normalize(t);
	glm::vec3 v = glm::vec3(t.z, t.y, -t.x);
	glm::vec3 u = glm::cross(t, v);
	if (moveForward)
	{
		pos += speed*deltaTime*t;
	}
	if (moveBackward)
	{
		pos -= speed*deltaTime*t;
	}
	if (moveRight)
	{
		pos += speed*deltaTime*v;
	}
	if (moveLeft)
	{
		pos -= speed*deltaTime*v;
	}
	if (moveUp)
	{
		pos += speed*deltaTime*u;
	}
	if (moveDown)
	{
		pos -= speed*deltaTime*u;
	}

	position = pos;
	computeAABB();
	updateCamera();
}

//Update the camera along the player position
void Player::updateCamera()
{
    cam->setPosition( position );
}

//Callback for the mouse
void Player::mouseMotion(int x, int y)
{
	if (canMove)
	{
		if (firstTime)
		{
			lastX = x;
			lastY = y;
			firstTime = false;
		}

		int xoff = x - lastX;
		int yoff = y - lastY;
		lastX = x;
		lastY = y;

		float sensitivity = 0.5f;
		xoff *= sensitivity;
		yoff *= -sensitivity;

		cam->addYaw(xoff);
		cam->addPitch(yoff);
		cam->updateCameraVectors();
	}
}

//Compute AABB for the player (the player having no model we cant auto compute it
void Player::computeAABB()
{
    float halfSize = 0.2f;
    aabb.max.x = position.x + halfSize;
    aabb.max.y = position.y + halfSize;
    aabb.max.z = position.z + halfSize;
    aabb.min.x = position.x - halfSize;
    aabb.min.y = position.y - 0.4f;
    aabb.min.z = position.z - halfSize;

    aabb.size = glm::vec3(halfSize*2);
    aabb.center = position;
}
