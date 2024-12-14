#pragma once
class Timer
{
public:
	void UpdateFPS();

private:
	int frameCount = 0;
	float fps = 0.0f;
	int previousTime = 0;
};

