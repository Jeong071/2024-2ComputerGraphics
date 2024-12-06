#include "pch.h"
#include "Camera.h"
//카메라 관련

Camera::Camera()
    : position(glm::vec3(gInput.GetPlayerXPos() + 0.0f, gInput.GetPlayerYPos() +  1.3f, gInput.GetPlayerZPos() + 3.0f)), 
    target(glm::vec3(2 * gInput.GetPlayerXPos() - position[0],gInput.GetPlayerYPos(),2 * gInput.GetPlayerZPos() - position[2])), 
    upDirection(glm::vec3(0.0f, 1.0f, 0.0f)) {
}

Camera::~Camera() {
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, upDirection);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const {
    return glm::perspective(glm::radians(70.0f), width / height, 0.1f, 100.0f);
}

void Camera::SetCameraPos(float x, float y, float z)
{
    position = glm::vec3(x, y + 1.3f, z + 3.0f);
}

void Camera::SetCameraTarget(float x, float y, float z)
{
    target = glm::vec3(2 * gInput.GetPlayerXPos() - position[0],
                        gInput.GetPlayerYPos(),
                        2 * gInput.GetPlayerZPos() - position[2]);
}
