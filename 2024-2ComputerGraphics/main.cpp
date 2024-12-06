#define _CRTDBG_MAP_ALLOC
#include "pch.h"
#include "Renderer.h"

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::mat4 rotMatrix = glm::mat4(1.0f);

Model gModel;


int main(int argc, char** argv) {

#if defined(_DEBUG)
	// CRT 메모리 누수 검출을 활성화합니다.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	glutInit(&argc, argv);
	Initialize init(0, 0, WIDTH, HEIGHT);
	init.GLEWInit();

	gModel.ParseObj("Player.obj");
	
    
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	Renderer draw;
	draw.CreateShader();
	glutDisplayFunc(draw.RenderScene);
    
	gInput.Run();
	
#if defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif
	glutMainLoop();
}