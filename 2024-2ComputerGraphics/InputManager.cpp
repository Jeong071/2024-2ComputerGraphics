#include "pch.h"
#include "InputManager.h"
//�Է� Ű ó��

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


