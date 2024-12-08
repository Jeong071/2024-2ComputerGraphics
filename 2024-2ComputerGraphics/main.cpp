#define _CRTDBG_MAP_ALLOC
#include "pch.h"
#include "Renderer.h"

GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::mat4 rotMatrix = glm::mat4(1.0f);

Model gModel;


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	Initialize init(0, 0, WIDTH, HEIGHT);
	init.GLEWInit();

	gModel.ParseObj("Player.obj");
	
    
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	Renderer draw;
	draw.CreateShader();
	glutDisplayFunc(draw.RenderScene);
    
	gInput.Run();
	
	glutMainLoop();
}