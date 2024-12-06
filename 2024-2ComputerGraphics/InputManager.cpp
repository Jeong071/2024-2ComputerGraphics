#include "pch.h"
#include "InputManager.h"
//입력 키 처리

GLvoid InputManager::Key(unsigned char key, int x, int y)
{
	if (key == 'w') {
		mPlayerZPos -= PLAYER_SPEED;
	}
	if (key == 's') {
		mPlayerZPos += PLAYER_SPEED;
	}
	if (key == 'a') {
		mPlayerXPos -= PLAYER_SPEED;
	}
	if (key == 'd') {
		mPlayerXPos += PLAYER_SPEED;
	}
	
	

	glutPostRedisplay();
}


