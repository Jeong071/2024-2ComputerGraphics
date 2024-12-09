#include "pch.h"
#include "Player.h"

void Player::Rotate(glm::vec3& first, glm::vec3& last, float turnAngle)
{

    glm::vec3 rotationAxis = glm::cross(first, last);

    if (rotationAxis.y >= 0.0f) {
        rotateYAxis = -1.0f;
    }
    else {
        rotateYAxis = 1.0f;
    }

    rotationAxis = glm::normalize(rotationAxis);
    float dotProduct = glm::dot(first, last);


    dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);

    float angle = glm::degrees(acos(dotProduct));

    playerAngle += 5.0f;

    if (playerAngle + 5.0f > angle) {
        playerAngle = angle;
    }
    else {
        playerAngle += 5.0f;
    }
    last = first;

}