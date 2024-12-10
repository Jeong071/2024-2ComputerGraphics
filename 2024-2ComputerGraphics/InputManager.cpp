#include "pch.h"
#include "InputManager.h"
#include "Player.h"
//입력 키 처리
bool isVirtualMouse{ true };

float firstAngle = playerAngle;
glm::vec3 rotationAxis;
float rotateYAxis{1.0f};
float jumpVelocity = 0.0f;
const float gravity = -0.98f; // 중력 가속도 
float positionY = 0.0f;

glm::vec3 stage1 = glm::vec3(0.0f, 0.1f, 8.0f);
glm::vec3 stage2 = glm::vec3(0.0f, 0.1f, -14.0f);
glm::vec3 stage3 = glm::vec3(0.0f, 0.1f, 8.0f);

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
    if (key == ' ' && !gPlayer.GetIsJumping() && !gPlayer.GetIsFalling()) {
        gPlayer.SetIsJumping(true);
        jumpVelocity = 0.2f; // 점프 속도
        
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

   gPlayer.SetYaw(gPlayer.GetYaw() + dx);
   gPlayer.SetPitch(gPlayer.GetPitch() + dy);

    if (gPlayer.GetPitch() > 89.0f)
        gPlayer.SetPitch(89.0f);
    if (gPlayer.GetPitch() < 15.0f)
        gPlayer.SetPitch(15.0f);

    if (isVirtualMouse) {
        glutWarpPointer(WIDTH / 2, HEIGHT / 2);
    }

    glutPostRedisplay();
}

GLvoid InputManager::Timer(int value)
{
    float firstAngle = playerAngle;
    float yaw = gPlayer.GetYaw();
    glm::vec3 cameraVec = glm::vec3(gCamera.GetTarget().x - gCamera.GetCameraPos().x, 0.0f, gCamera.GetTarget().z - gCamera.GetCameraPos().z);
    cameraVec = glm::normalize(cameraVec);
    glm::vec3 playerVec = glm::vec3(gPlayer.GetPlayerLookVec().x, 0.0f, gPlayer.GetPlayerLookVec().z);
    playerVec = glm::normalize(playerVec);
    
    if (mKeys['w'] && mKeys['a'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, -45.0f);
        gPlayer.MovePlayerXPos(- PLAYER_SPEED * cos(glm::radians(yaw - 45.0f)));
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 45.0f)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['w'] && mKeys['d'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, 45.0f);
        gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 45.0f)));
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 45.0f)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['s'] && mKeys['a'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, -135.0f);
        gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw - 135.0f)));
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 135.0f)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['s'] && mKeys['d'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, 135.0f);
        gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 135.0f)));
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 135.0f)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }

    else if (mKeys['w'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, 0.0f);
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
        gPlayer.MovePlayerXPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['s'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, 180.0f);
        gPlayer.MovePlayerXPos(PLAYER_SPEED * cos(glm::radians(yaw)));
        gPlayer.MovePlayerZPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['a'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, -90.0f);
        gPlayer.MovePlayerXPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
        gPlayer.MovePlayerZPos(PLAYER_SPEED * cos(glm::radians(yaw)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    else if (mKeys['d'] && !gPlayer.GetIsFalling()) {
        gPlayer.Rotate(cameraVec, playerVec, 90.0f);
        gPlayer.MovePlayerXPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
        gPlayer.MovePlayerZPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    }
    

    if (mKeys['z']) {
        std::cout << "플레이어 min : " << gPlayer.GetMinPoint().x<<", " << gPlayer.GetMinPoint().y << ", " << gPlayer.GetMinPoint().z << std::endl;
        std::cout << "플레이어 max : " << gPlayer.GetMaxPoint().x << ", " << gPlayer.GetMaxPoint().y << ", " << gPlayer.GetMaxPoint().z << std::endl;
        
    }
    if (gPlayer.GetIsJumping()) {
        jumpVelocity += gravity * 1.0f/75.0f;
        gPlayer.MovePlayerYPos(jumpVelocity);
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());

        // 바닥에 도착했을 때
        if (gPlayer.GetPlayerYPos() <= 0.f) {
            gPlayer.SetPlayerYPos(0.1f);
            gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
            gPlayer.SetIsJumping(false);
            gPlayer.SetIsFalling(false);
            jumpVelocity = 0.0f;
        }
    }

    if (gPlayer.GetIsFalling()) {
        
        float FallVelocity{ 0.0f };
        
        FallVelocity += (gravity * 1.0f / 30.0f);
        
        gPlayer.MovePlayerYPos(FallVelocity);
        gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
        if (gPlayer.GetPlayerYPos() <= -1.5f) {
            if (gPlayer.GetStage() == 1) {
                gPlayer.SetPlayerXPos(stage1.x);
                gPlayer.SetPlayerYPos(stage1.y);
                gPlayer.SetPlayerZPos(stage1.z);
                
                
            }
            else if (gPlayer.GetStage() == 2) {
                gPlayer.SetPlayerXPos(stage2.x);
                gPlayer.SetPlayerYPos(stage2.y);
                gPlayer.SetPlayerZPos(stage2.z);
              
            }
            gPlayer.SetYaw(90.0f);
            playerAngle = 0.0f;
        }
    }

    if (gPlayer.GetPlayerZPos() < 15.0f && gPlayer.GetPlayerZPos() >= -4.0f) {
        gPlayer.SetStage(1);
    }
    else if (gPlayer.GetPlayerZPos() < -4.0f && gPlayer.GetPlayerZPos() >= -12.0f) {
        gPlayer.SetStage(2);
    }

    glutTimerFunc(20, gInput.Timer, 0);
    glutPostRedisplay();
}




