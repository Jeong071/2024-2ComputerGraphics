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
	void ResetPlayerLookVec() { mPlayerLookVec = glm::vec3(0.0f, 0.0f, -1.0f); }

	float GetYaw() { return mYaw; }
	float GetPitch() { return mPitch; }

	void SetYaw(float yaw) { mYaw = yaw; }
	void SetPitch(float pitch) { mPitch = pitch; }

	bool GetIsJumping() { return isJumping; }
	void SetIsJumping(bool isJump) { isJumping = isJump; }

	bool GetIsFalling() { return isFalling; }
	void SetIsFalling(bool isFall) { isFalling = isFall; }

	bool GetIsDeath() { return isDeath; }
	void SetIsDeath(bool Death) { isDeath = Death; }

	bool GetIsOnMovingFloor() { return isOnMovingFloor; }
	void SetIsOnMovingFloor(bool moving) { isOnMovingFloor = moving; }
	
	bool GetIsGoal() { return isGoal; }
	void SetIsGoal(bool goal) { isGoal = goal; }

	void SetAABB(float x, float y, float z);
	glm::vec3 GetMinPoint() { return mMinPoint; }
	glm::vec3 GetMaxPoint() { return mMaxPoint; }
	void PrintPlayerAABB();
	void PrintPlayerPos();

	void SetStage(int a) { mPlayerStage = a; }
	int GetStage() { return mPlayerStage; }

	glm::vec3 CreateNewMinAABB(float x, float z);
	glm::vec3 CreateNewMaxAABB(float x, float z);

private:
	float mPlayerXPos{ 0.0f };
	float mPlayerYPos{ 0.1f };
	float mPlayerZPos{ -24.0f };

	bool isJumping{ false };
	bool isFalling{ false };
	bool isDeath{ false };
	bool isOnMovingFloor{ false };
	bool isGoal{ false };

	glm::vec3 mPlayerLookVec{ 0.0f, 0.0f, -1.0f };

	float mYaw{ 90.0f };
	float mPitch{ 10.0f };
	
	int mPlayerStage {1};

	glm::vec3 mMinPoint{-0.05f,0.1f,7.95f};
	glm::vec3 mMaxPoint{0.05f,0.3f,8.05f};

	
};

