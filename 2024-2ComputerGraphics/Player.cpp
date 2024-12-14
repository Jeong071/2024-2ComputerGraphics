#include "pch.h"
#include "Player.h"

void Player::Rotate(glm::vec3& first, glm::vec3& last, float turnAngle)
{
    glm::vec4 temp = glm::vec4(first,1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    rot = rotate(rot, glm::radians(turnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    temp = temp * rot;
    glm::vec3 firstAngle = glm::vec3(temp);
    

    glm::vec3 rotationAxis = glm::cross(firstAngle, last);

    if (rotationAxis.y >= 0.0f) {
        rotateYAxis = -1.0f;
    }
    else {
        rotateYAxis = 1.0f;
    }

    rotationAxis = glm::normalize(rotationAxis);
    float dotProduct = glm::dot(firstAngle, last);

    dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);

    float angle = glm::degrees(acos(dotProduct));

    playerAngle += 5.0f;

    if (playerAngle + 5.0f > angle) {
        playerAngle = angle;
    }
    else {
        playerAngle += 5.0f;
    }
    

}

void Player::SetAABB(float x, float y, float z)
{
    mMinPoint.x = x - 0.05f;
    mMinPoint.y = y;
    mMinPoint.z = z - 0.05f;

    mMaxPoint.x = x + 0.05f;
    mMaxPoint.y = y + 0.1f;
    mMaxPoint.z = z + 0.05f;
}

void Player::PrintPlayerAABB()
{
    std::cout<<"min - " << mMinPoint.x << ", " << mMinPoint.y << ", " << mMinPoint.z << std::endl;
    std::cout << "max - " << mMaxPoint.x << ", " << mMaxPoint.y << ", " << mMaxPoint.z << std::endl;
}

void Player::PrintPlayerPos()
{
    std::cout << mPlayerXPos << ", " << mPlayerYPos << ", " << mPlayerZPos << std::endl;
}

glm::vec3 Player::CreateNewMinAABB(float x, float z)
{
    glm::vec3 result;
    result.x = std::min(gPlayer.GetMinPoint().x, gPlayer.GetMinPoint().x + x);
    result.y = gPlayer.GetMinPoint().y;
    result.z = std::min(gPlayer.GetMinPoint().z, gPlayer.GetMinPoint().z + z);

    return result;
}

glm::vec3 Player::CreateNewMaxAABB(float x, float z)
{
    glm::vec3 result;
    result.x = std::max(gPlayer.GetMaxPoint().x, gPlayer.GetMaxPoint().x + x);
    result.y = gPlayer.GetMinPoint().y;
    result.z = std::max(gPlayer.GetMaxPoint().z, gPlayer.GetMaxPoint().z + z);

    return result;
}
