#include "pch.h"
#include "Camera.h"
//카메라 관련

Camera::Camera()
    : position(glm::vec3(0.0f, 15.0f, 0.1f)), target(glm::vec3(0.0f, 0.0f, 0.0f)), upDirection(glm::vec3(0.0f, 1.0f, 0.0f)) {
}

Camera::~Camera() {
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, upDirection);
}

glm::mat4 Camera::getProjectionMatrix(float width, float height) const {
    return glm::perspective(glm::radians(70.0f), width / height, 0.1f, 100.0f);
}