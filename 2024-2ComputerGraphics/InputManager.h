#pragma once
class InputManager
{
public:
	static GLvoid Key(unsigned char key, int x, int y);
	static GLvoid MouseMotion(int x, int y);
	static GLvoid Timer(int value);

	static void Rotate(glm::vec3& first, glm::vec3& last, float turnAngle);

	static void Run();

	static float GetPlayerXPos() { return mPlayerXPos; }
	static float GetPlayerYPos() { return mPlayerYPos; }
	static float GetPlayerZPos() { return mPlayerZPos; }

	static glm::vec3 GetPlayerLookVec() { return mPlayerLookVec; }

	static float GetYaw() { return mYaw; }
	static float GetPitch() { return mPitch; }
private:
	inline static float mPlayerXPos	{ 0.0f };
	inline static float mPlayerYPos	{ 0.0f };
	inline static float mPlayerZPos	{ 0.0f };

	inline static glm::vec3 mPlayerLookVec{ 0.0f, 0.0f, -1.0f };

	inline static float mYaw		{90.0f};
	inline static float mPitch		{10.0f};


	
};

