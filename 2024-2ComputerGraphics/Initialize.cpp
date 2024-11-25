#include "pch.h"
#include "Initialize.h"
//�ʱ�ȭ

Initialize::Initialize(int XPos, int YPos, int XSize, int YSize)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE); // ���÷��� ��� ����
	glutInitWindowPosition(XPos, YPos); // �������� ��ġ ����
	glutInitWindowSize(XSize, YSize); // �������� ũ�� ����
	glutCreateWindow("GL");
}

Initialize::~Initialize()
{
}

GLvoid Initialize::GLEWInit()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW Initialized\n";
	}
}
