#pragma once
class Player {
public:
	void Rotate(glm::vec3& first, glm::vec3& last, float turnAngle);

	float GetPlayerXPos() { return mPlayerXPos; }
	float GetPlayerYPos() { return mPlayerYPos; }
	float GetPlayerZPos() { return mPlayerZPos; }

	void MovePlayerXPos(float x) { SetPlayerXPos(mPlayerXPos + x); }
	void MovePlayerYPos(float y) { SetPlayerYPos(mPlayerYPos + y); }
	void MovePlayerZPos(float z) { SetPlayerZPos(mPlayerZPos + z); }

	void SetPlayerXPos(float x) { mPlayerXPos = x; }
	void SetPlayerYPos(float y) { mPlayerYPos = y; }
	void SetPlayerZPos(float z) { mPlayerZPos = z; }

	glm::vec3 GetPlayerLookVec() { return mPlayerLookVec; }

	float GetYaw() { return mYaw; }
	float GetPitch() { return mPitch; }

	void SetYaw(float yaw) { mYaw = yaw; }
	void SetPitch(float pitch) { mPitch = pitch; }

	bool GetIsJumping() { return isJumping; }
	
	void SetIsJumping(bool isJump) { isJumping = isJump; }
private:
	float mPlayerXPos{ 0.0f };
	float mPlayerYPos{ 0.6f };
	float mPlayerZPos{ 8.0f };

	bool isJumping{ false };

	glm::vec3 mPlayerLookVec{ 0.0f, 0.0f, -1.0f };

	float mYaw{ 90.0f };
	float mPitch{ 10.0f };

	
};

