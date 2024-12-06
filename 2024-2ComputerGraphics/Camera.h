#pragma once

class Camera {
public:
    Camera();
    ~Camera();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float width, float height) const;

    void SetCameraPos(float x, float y, float z);
    void SetCameraTarget(float x, float y, float z);

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 upDirection;
};
