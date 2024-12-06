#include "pch.h"
#include "InputManager.h"
//입력 키 처리
bool isVirtualMouse{ true };

float firstAngle = playerAngle;
glm::vec3 rotationAxis;
float rotateYAxis{1.0f};
void InputManager::Rotate(glm::vec3& first, glm::vec3& last, float turnAngle)
{
    glm::vec3 rotationAxis = glm::cross(first, last);
   
    if (rotationAxis.y > 0.0f) {
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

    if (playerAngle > 180)
        playerAngle -= 360.f;

    if (playerAngle < -180.f)
        playerAngle += 360.f;

    if (playerAngle > angle) {
        playerAngle = angle;
    }
    first = last;
    //std::cout << playerAngle << std::endl;
}
void InputManager::Run()
{
    glutKeyboardFunc(Key);
    glutPassiveMotionFunc(MouseMotion);
    glutTimerFunc(20, Timer, 0);
}
GLvoid InputManager::Key(unsigned char key, int x, int y)
{
    float firstAngle = playerAngle;
    float yaw = gInput.GetYaw();
    glm::vec3 cameraVec = glm::normalize(gCamera.GetTarget() - gCamera.GetCameraPos());
    glm::vec3 playerVec = glm::vec3(mPlayerLookVec.x, 0.0f, mPlayerLookVec.z);
    playerVec = glm::normalize(playerVec);
	if (key == 'w') {
        
        Rotate(cameraVec, playerVec, 0.0f);
       
		mPlayerZPos -= PLAYER_SPEED *  sin(glm::radians(yaw));
        mPlayerXPos += PLAYER_SPEED * - cos(glm::radians(yaw));
        
	}
	if (key == 's') {
        
        
        mPlayerZPos -= PLAYER_SPEED * -sin(glm::radians(yaw));
        mPlayerXPos += PLAYER_SPEED *  cos(glm::radians(yaw));
	}
	if (key == 'a') {
      
        
        mPlayerZPos += PLAYER_SPEED * cos(glm::radians(yaw));
        mPlayerXPos -= PLAYER_SPEED * sin(glm::radians(yaw));
	}
	if (key == 'd') {
       
        mPlayerZPos += PLAYER_SPEED * -cos(glm::radians(yaw));
        mPlayerXPos -= PLAYER_SPEED * -sin(glm::radians(yaw));
	}
    if (key == 'v') {
        isVirtualMouse = !isVirtualMouse;
    }

    
	
    
	glutPostRedisplay();
}



GLvoid InputManager::MouseMotion(int x, int y)
{
    // 마우스 이동 델타 계산 (현재 위치 - 중앙 위치)
    int dx = x - WIDTH / 2;
    int dy = y - HEIGHT / 2;

    // 델타를 민감도에 맞게 조정
    dx *= SENSITIVITY;
    dy *= SENSITIVITY;

    // 카메라 각도 업데이트
   mYaw += dx;
   mPitch += dy;

    // pitch 제한 (카메라가 위 또는 아래로 너무 많이 회전하지 않도록)
    if (mPitch > 89.0f)
        mPitch = 89.0f;
    if (mPitch < 0.0f)
        mPitch = 0.0f;

    if (isVirtualMouse) {
        glutWarpPointer(WIDTH / 2, HEIGHT / 2);
    }
    // 화면 다시 그리기 요청
    glutPostRedisplay();
}

GLvoid InputManager::Timer(int value)
{
    
    glutTimerFunc(20, gInput.Timer, 0);
    glutPostRedisplay();
}




