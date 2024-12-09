#pragma once
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

private:

	inline static unsigned char mKeys[256]{ false };
	

	
};

