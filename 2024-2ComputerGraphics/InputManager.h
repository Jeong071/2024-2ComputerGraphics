#pragma once
class InputManager
{
public:
	static GLvoid Key(unsigned char key, int x, int y);
	

	static float GetPlayerXPos() { return mPlayerXPos; }
	static float GetPlayerYPos() { return mPlayerYPos; }
	static float GetPlayerZPos() { return mPlayerZPos; }

	static float GetPlayerAngle() { return mPlayerAngle; }
private:
	inline static float mPlayerXPos	{ 0.0f };
	inline static float mPlayerYPos	{ 0.0f };
	inline static float mPlayerZPos	{ 0.0f };

	inline static float mPlayerAngle{ 0.0f };

	
};

