#include "pch.h"
#include "Initialize.h"
//초기화

Initialize::Initialize(int XPos, int YPos, int XSize, int YSize)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE); // 디스플레이 모드 설정
	glutInitWindowPosition(XPos, YPos); // 윈도우의 위치 지정
	glutInitWindowSize(XSize, YSize); // 윈도우의 크기 지정
	glutCreateWindow("GL");
}

Initialize::~Initialize()
{
}

GLvoid Initialize::GLEWInit()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW Initialized\n";
	}
}
