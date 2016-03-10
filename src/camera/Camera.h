#if !defined(CAMERA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */

#define CAMERA_H

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public :
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = 90.0f, GLfloat pitch = 0.0f);

    glm::mat4 getViewMatrix();
    glm::mat4 getRotatedViewMatrix();
    GLfloat getZoom() { return(zoom); }
    void setPosition( glm::vec3 newPos ) { position = newPos; }
    glm::vec3 getPosition(){return(position);}
    void addYaw( float value ); 
    void addPitch( float value );
    void updateCameraVectors();
    glm::vec3 getFront(){return(front);}
private:
    glm::vec3 position;
    glm::vec3 up;
    GLfloat yaw;
    GLfloat pitch;
    GLfloat zoom;
    glm::vec3 front;
    glm::vec3 right;

    
};

#endif
