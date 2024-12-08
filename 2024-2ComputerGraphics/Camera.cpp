#include "pch.h"
#include "Camera.h"
//ī�޶� ����

Camera::Camera()
    : position(glm::vec3(gInput.GetPlayerXPos() , gInput.GetPlayerYPos(), gInput.GetPlayerZPos())), 
    target(normalize(glm::vec3(2 * gInput.GetPlayerXPos() - position[0],gInput.GetPlayerYPos(),2 * gInput.GetPlayerZPos() - position[2]))), 
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

glm::vec3 Camera::GetTarget() const
{
    return target;
}

glm::vec3 Camera::GetCameraPos() const
{
    return position;
}

void Camera::SetCameraPos(float x, float y, float z)
{
    float radYaw = glm::radians(gInput.GetYaw());
    float radPitch = glm::radians(gInput.GetPitch());

    float cx = DISTANCE * cos(radPitch) * cos(radYaw);
    float cy = DISTANCE * sin(radPitch);
    float cz = DISTANCE * cos(radPitch) * sin(radYaw);

    position = glm::vec3(x + cx, y + cy, z + cz);
    
    
}

void Camera::SetCameraTarget(float x, float y, float z)
{
    target = glm::vec3(2 * x - position[0],
                        2 * y - position[1],
                        2 * z - position[2]);
}
