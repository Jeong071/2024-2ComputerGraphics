#pragma once
class Initialize {

public:
	Initialize(int XPos, int YPos, int XSize, int YSize);
	~Initialize();

	GLvoid GLEWInit();

private:
	int XPos	{ 0 };
	int YPos	{ 0 };
	int XSize	{ 0 };
	int YSize	{ 0 };
};

