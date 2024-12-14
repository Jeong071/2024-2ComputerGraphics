#include "pch.h"
#include "InputManager.h"
#include "Player.h"
//입력 키 처리
bool isVirtualMouse{ true };

float firstAngle = playerAngle;
glm::vec3 rotationAxis{};
float rotateYAxis{1.0f};
float jumpVelocity = 0.0f;
const float gravity = -0.98f; // 중력 가속도 
float positionY = 0.0f;

glm::vec3 stage1 = glm::vec3(0.0f, 0.1f, 8.0f);
glm::vec3 stage2 = glm::vec3(0.0f, 0.1f, -4.0f);
glm::vec3 stage3 = glm::vec3(0.0f, 0.1f, -14.0f);


InputManager::InputManager()
{

}
InputManager::~InputManager()
{

}

void InputManager::Run()
{
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(MouseMotion);
    glutTimerFunc(16, Timer, 0);
}
bool InputManager::IsKeyPressed(unsigned char key) //직전 프레임 X - 현재 프레임 O
{
    return !mPreviousKeyState[key] && mCurrentKeyState[key];
}
bool InputManager::IsKeyReleased(unsigned char key) //직전 프레임 O - 현재 프레임 X
{
    return mPreviousKeyState[key] && !mCurrentKeyState[key];

}
bool InputManager::IsKeyUp(unsigned char key) //현재 프레임 O
{
    return !mCurrentKeyState[key];
}
bool InputManager::IsKeyDown(unsigned char key) //현재 프레임 X
{
    return mCurrentKeyState[key];
}

void InputManager::UpdateKeyState()
{
    for (int i = 0; i < KEY_COUNT; ++i) {
        mPreviousKeyState[i] = mCurrentKeyState[i];
    }
}

bool InputManager::CheckCollision(glm::vec3 newMinAABB, glm::vec3 newMaxAABB, std::vector<Cube>& cube)
{
    for (Cube& b : cube) {
        if ((newMaxAABB.x > b.GetCubeMinAABB().x && newMinAABB.x < b.GetCubeMaxAABB().x) &&
            (newMaxAABB.y > b.GetCubeMinAABB().y && newMinAABB.y < b.GetCubeMaxAABB().y) &&
            (newMaxAABB.z > b.GetCubeMinAABB().z && newMinAABB.z < b.GetCubeMaxAABB().z)) {
            if (gPlayer.GetPlayerZPos() - 0.05f > b.GetCubeMaxAABB().z || gPlayer.GetPlayerZPos() + 0.05f < b.GetCubeMinAABB().z) mWallNum = 0;
            else if (gPlayer.GetPlayerXPos() > b.GetCubeMaxAABB().x || gPlayer.GetPlayerXPos() < b.GetCubeMinAABB().x) mWallNum = 1;
            return true;
        }
    };
    return false;
}



GLvoid InputManager::Key(unsigned char key, int x, int y)
{
    mCurrentKeyState[key] = true;
}

GLvoid InputManager::keyUp(unsigned char key, int x, int y)
{
    mCurrentKeyState[key] = false;
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
    glm::vec3 newMinAABB;
    glm::vec3 newMaxAABB;
    std::vector<Cube> cube = gRenderer.GetCubeIdx();
    if (currentScene == GAME_PLAY) {
        if (IsKeyDown('w') && IsKeyDown('a') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * cos(glm::radians(yaw - 45.0f)), -PLAYER_SPEED * sin(glm::radians(yaw - 45.0f)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * cos(glm::radians(yaw - 45.0f)), -PLAYER_SPEED * sin(glm::radians(yaw - 45.0f)));
            gPlayer.Rotate(cameraVec, playerVec, -45.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw - 45.0f)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 45.0f)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw - 45.0f)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 45.0f)));
                }
            }


        }
        else if (IsKeyDown('w') && IsKeyDown('d') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * cos(glm::radians(yaw + 45.0f)), -PLAYER_SPEED * sin(glm::radians(yaw + 45.0f)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * cos(glm::radians(yaw + 45.0f)), -PLAYER_SPEED * sin(glm::radians(yaw + 45.0f)));
            gPlayer.Rotate(cameraVec, playerVec, 45.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 45.0f)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 45.0f)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 45.0f)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 45.0f)));
                }
            }
        }
        else if (IsKeyDown('s') && IsKeyDown('a') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * cos(glm::radians(yaw - 135.0f)), -PLAYER_SPEED * sin(glm::radians(yaw - 135.0f)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * cos(glm::radians(yaw - 135.0f)), -PLAYER_SPEED * sin(glm::radians(yaw - 135.0f)));
            gPlayer.Rotate(cameraVec, playerVec, -135.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw - 135.0f)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 135.0f)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw - 135.0f)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw - 135.0f)));
                }
            }

        }
        else if (IsKeyDown('s') && IsKeyDown('d') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * cos(glm::radians(yaw + 135.0f)), -PLAYER_SPEED * sin(glm::radians(yaw + 135.0f)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * cos(glm::radians(yaw + 135.0f)), -PLAYER_SPEED * sin(glm::radians(yaw + 135.0f)));
            gPlayer.Rotate(cameraVec, playerVec, 135.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 135.0f)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 135.0f)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * cos(glm::radians(yaw + 135.0f)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw + 135.0f)));
                }
            }
        }

        else if (IsKeyDown('w') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(PLAYER_SPEED * -cos(glm::radians(yaw)), -PLAYER_SPEED * sin(glm::radians(yaw)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(PLAYER_SPEED * -cos(glm::radians(yaw)), -PLAYER_SPEED * sin(glm::radians(yaw)));
            gPlayer.Rotate(cameraVec, playerVec, 0.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
                }
            }

        }
        else if (IsKeyDown('s') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(PLAYER_SPEED * cos(glm::radians(yaw)), -PLAYER_SPEED * -sin(glm::radians(yaw)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(PLAYER_SPEED * cos(glm::radians(yaw)), -PLAYER_SPEED * -sin(glm::radians(yaw)));
            gPlayer.Rotate(cameraVec, playerVec, 180.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(PLAYER_SPEED * cos(glm::radians(yaw)));
                gPlayer.MovePlayerZPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(PLAYER_SPEED * cos(glm::radians(yaw)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
                }
            }

        }
        else if (IsKeyDown('a') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * sin(glm::radians(yaw)), PLAYER_SPEED * cos(glm::radians(yaw)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * sin(glm::radians(yaw)), PLAYER_SPEED * cos(glm::radians(yaw)));
            gPlayer.Rotate(cameraVec, playerVec, -90.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
                gPlayer.MovePlayerZPos(PLAYER_SPEED * cos(glm::radians(yaw)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * sin(glm::radians(yaw)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(PLAYER_SPEED * cos(glm::radians(yaw)));
                }
            }
        }
        else if (IsKeyDown('d') && !gPlayer.GetIsFalling()) {
            newMinAABB = gPlayer.CreateNewMinAABB(-PLAYER_SPEED * -sin(glm::radians(yaw)), PLAYER_SPEED * -cos(glm::radians(yaw)));
            newMaxAABB = gPlayer.CreateNewMaxAABB(-PLAYER_SPEED * -sin(glm::radians(yaw)), PLAYER_SPEED * -cos(glm::radians(yaw)));
            gPlayer.Rotate(cameraVec, playerVec, 90.0f);
            if (!CheckCollision(newMinAABB, newMaxAABB, cube)) {
                gPlayer.MovePlayerXPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
                gPlayer.MovePlayerZPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
            }
            else {
                if (mWallNum == 0) {
                    gPlayer.MovePlayerXPos(-PLAYER_SPEED * -sin(glm::radians(yaw)));
                }
                else if (mWallNum == 1) {
                    gPlayer.MovePlayerZPos(PLAYER_SPEED * -cos(glm::radians(yaw)));
                }
            }

        }

        if (IsKeyPressed(' ') && !gPlayer.GetIsJumping() && !gPlayer.GetIsFalling()) {

            gPlayer.SetIsJumping(true);

            jumpVelocity = 0.16f;
        }

        if (IsKeyPressed('\r') && currentScene == MAIN_MENU) {
            gPlayer.SetYaw(90.0f);
            playerAngle = 0.0f;
            currentScene = GAME_PLAY;
        }

        if (IsKeyPressed('z') && currentScene == GAME_PLAY) {
            gPlayer.SetYaw(90.0f);
            playerAngle = 0.0f;
            gPlayer.SetPlayerXPos(stage1.x);
            gPlayer.SetPlayerYPos(stage1.y);
            gPlayer.SetPlayerZPos(stage1.z);
            currentScene = MAIN_MENU;
        }
    }
    if (IsKeyPressed('\r') && currentScene == MAIN_MENU) {
        gPlayer.SetYaw(90.0f);
        playerAngle = 0.0f;
        currentScene = GAME_PLAY;
    }

    if (IsKeyPressed('v')) {
        isVirtualMouse = !isVirtualMouse;
    }

    if (gPlayer.GetIsGoal()) {
        gPlayer.SetYaw(90.0f);
        playerAngle = 0.0f;
        gPlayer.SetStage(1);
        gPlayer.SetPlayerXPos(stage1.x);
        gPlayer.SetPlayerYPos(stage1.y);
        gPlayer.SetPlayerZPos(stage1.z);
        gPlayer.SetIsGoal(false);
    }
    
    if (gPlayer.GetIsJumping()) {
        jumpVelocity += gravity * 1.0f/75.0f;
        gPlayer.MovePlayerYPos(jumpVelocity);
       

        // 바닥에 도착했을 때
        if (gPlayer.GetPlayerYPos() <= 0.f) {
            gPlayer.SetPlayerYPos(0.1f);
            
            gPlayer.SetIsJumping(false);
            gPlayer.SetIsFalling(false);
            jumpVelocity = 0.0f;
        }
        
    }
    if (gPlayer.GetIsDeath()) {
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
        else if (gPlayer.GetStage() == 3) {
            gPlayer.SetPlayerXPos(stage3.x);
            gPlayer.SetPlayerYPos(stage3.y);
            gPlayer.SetPlayerZPos(stage3.z);

        }
        gPlayer.SetIsDeath(false);
        gPlayer.SetYaw(90.0f);
        playerAngle = 0.0f;
    }
    if (gPlayer.GetIsFalling()) {
        
        float FallVelocity{ 0.0f };
        
        FallVelocity += (gravity * 1.0f / 30.0f);
        
        gPlayer.MovePlayerYPos(FallVelocity);
        
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
            else if (gPlayer.GetStage() == 3) {
                gPlayer.SetPlayerXPos(stage3.x);
                gPlayer.SetPlayerYPos(stage3.y);
                gPlayer.SetPlayerZPos(stage3.z);

            }
            gPlayer.SetIsDeath(false);
            gPlayer.SetYaw(90.0f);
            playerAngle = 0.0f;
        }
       
    }
    gPlayer.SetAABB(gPlayer.GetPlayerXPos(), gPlayer.GetPlayerYPos(), gPlayer.GetPlayerZPos());
    if (gPlayer.GetPlayerZPos() < 15.0f && gPlayer.GetPlayerZPos() >= -4.0f) {
        gPlayer.SetStage(1);
    }
    else if (gPlayer.GetPlayerZPos() < -4.0f && gPlayer.GetPlayerZPos() >= -14.0f) {
        gPlayer.SetStage(2);
    }
    else if (gPlayer.GetPlayerZPos() < -14.0f && gPlayer.GetPlayerZPos() >= -26.0f) {
        gPlayer.SetStage(3);
    }

    

    UpdateKeyState();
    glutTimerFunc(16, gInput.Timer, 0);
    glutPostRedisplay();
}




