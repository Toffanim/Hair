/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice:  $
   ======================================================================== */
#include "Camera.h"
//Camera class, pretty straightforward
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw , GLfloat pitch ) : position(position), up(up), yaw(yaw), pitch(pitch)
{
	updateCameraVectors();
    front = glm::vec3(1.0f, 0.0f, 0.0f);
    zoom = 45.0f;    
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
	return glm::lookAt(position, glm::vec3(0.f), up);
}

glm::mat4 Camera::getRotatedViewMatrix()
{
    glm::vec3 rotPos = glm::vec3( position.x, -position.y, position.z);
    glm::vec3 rotFront = glm::vec3( front.x , -front.y, front.z);
    return glm::lookAt(  rotPos, rotPos + rotFront, up);
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
}

void Camera::addYaw(float value)
{
    yaw += value;
    updateCameraVectors();
}


void Camera::addPitch( float value )
{
    pitch += value;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    updateCameraVectors();
}
