#include "pch.h"
#include "InputManager.h"
//입력 키 처리
bool isVirtualMouse{ true };
bool isJumping{ false };
float firstAngle = playerAngle;
glm::vec3 rotationAxis;
float rotateYAxis{1.0f};
float jumpVelocity = 0.0f;
const float gravity = -0.98f; // 중력 가속도 (조절 가능)
float positionY = 0.0f;

void InputManager::Rotate(glm::vec3& first, glm::vec3& last, float turnAngle)
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
void InputManager::Run()
{
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(MouseMotion);
    glutTimerFunc(20, Timer, 0);
}
InputManager::InputManager()
{

}
InputManager::~InputManager()
{

}
GLvoid InputManager::Key(unsigned char key, int x, int y)
{
    
	if (key == 'w') {
        mKeys[key] = true;
        
        
	}
	if (key == 's') {
        mKeys[key] = true;
       
	}
	if (key == 'a') {
        mKeys[key] = true;

	}
	if (key == 'd') {
        mKeys[key] = true;

	}
    if (key == 'v') {
        
        isVirtualMouse = !isVirtualMouse;
    }
    if (key == ' ' && !isJumping) { // 스페이스바
        isJumping = true;
        jumpVelocity = 0.3f; // 초기 점프 속도 (조절 가능)
        
    }
    if (key == 'z') {
        mKeys[key] = true;
    }
    
    
	
    
	glutPostRedisplay();
}

GLvoid InputManager::keyUp(unsigned char key, int x, int y)
{
    if (key == 'w') {
        mKeys[key] = false;
    }
    if (key == 's') {
        mKeys[key] = false;
    }
    if (key == 'a') {
        mKeys[key] = false;
    }
    if (key == 'd') {
        mKeys[key] = false;
    }
    if (key == 'z') {
        mKeys[key] = false;
    }
}



GLvoid InputManager::MouseMotion(int x, int y)
{
    
    int dx = x - WIDTH / 2;
    int dy = y - HEIGHT / 2;

    dx *= SENSITIVITY;
    dy *= SENSITIVITY;

   mYaw += dx;
   mPitch += dy;

    if (mPitch > 89.0f)
        mPitch = 89.0f;
    if (mPitch < 0.0f)
        mPitch = 0.0f;

    if (isVirtualMouse) {
        glutWarpPointer(WIDTH / 2, HEIGHT / 2);
    }

    glutPostRedisplay();
}

GLvoid InputManager::Timer(int value)
{
    float firstAngle = playerAngle;
    float yaw = gInput.GetYaw();
    glm::vec3 cameraVec = glm::normalize(gCamera.GetTarget() - gCamera.GetCameraPos());
    glm::vec3 playerVec = glm::vec3(mPlayerLookVec.x, 0.0f, mPlayerLookVec.z);
    playerVec = glm::normalize(playerVec);
    if (mKeys['w']) {
        Rotate(cameraVec, playerVec, 0.0f);
        mPlayerZPos -= PLAYER_SPEED * sin(glm::radians(yaw));
        mPlayerXPos += PLAYER_SPEED * -cos(glm::radians(yaw));

    }
    if (mKeys['s']) {
        mPlayerZPos -= PLAYER_SPEED * -sin(glm::radians(yaw));
        mPlayerXPos += PLAYER_SPEED * cos(glm::radians(yaw));
    }
    if (mKeys['a']) {
        mPlayerZPos += PLAYER_SPEED * cos(glm::radians(yaw));
        mPlayerXPos -= PLAYER_SPEED * sin(glm::radians(yaw));
    }
    if (mKeys['d']) {
        mPlayerZPos += PLAYER_SPEED * -cos(glm::radians(yaw));
        mPlayerXPos -= PLAYER_SPEED * -sin(glm::radians(yaw));
    }
    if (mKeys['z']) {
        std::cout << "플레이어 : " << playerVec.x << playerVec.y << playerVec.z << std::endl;
        std::cout << "카메라 : " << cameraVec.x << cameraVec.y << cameraVec.z << std::endl;
        std::cout << "=========================" << std::endl;
    }
    if (isJumping) {
        jumpVelocity += gravity * 1/60;
        mPlayerYPos += jumpVelocity;

        // 바닥에 도착했을 때
        if (mPlayerYPos <= 0.5f) {
            mPlayerYPos = 0.5f;
            isJumping = false;
            jumpVelocity = 0.0f;
        }
    }
    glutTimerFunc(20, gInput.Timer, 0);
    glutPostRedisplay();
}




