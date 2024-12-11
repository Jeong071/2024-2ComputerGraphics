#pragma once
#include "pch.h"
class InputManager
{
public:
	InputManager();
	~InputManager();

	static GLvoid Key(unsigned char key, int x, int y);
	static GLvoid keyUp(unsigned char key, int x, int y);
	static GLvoid MouseMotion(int x, int y);
	static GLvoid Timer(int value);
	static void Run();

	static bool IsKeyPressed(unsigned char key);
	static bool IsKeyReleased(unsigned char key);
	static bool IsKeyUp(unsigned char key);
	static bool IsKeyDown(unsigned char key);
	static void UpdateKeyState();

private:

	inline static unsigned char mCurrentKeyState[256]{ };
	inline static unsigned char mPreviousKeyState[256]{ };
	

	
};

