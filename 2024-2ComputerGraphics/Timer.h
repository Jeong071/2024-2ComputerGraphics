#pragma once

class Timer
{
public:
	Timer() {};
   explicit Timer(int interval);
    void start();
	void UpdateFPS();

private:
	int frameCount = 0;
	float fps = 0.0f;
	int previousTime = 0;
  int interval_;
   static void TimerFunc(int value);
};

