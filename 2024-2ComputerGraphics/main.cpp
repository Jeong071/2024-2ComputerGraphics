#include "pch.h"
#include "Renderer.h"
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::mat4 rotMatrix = glm::mat4(1.0f);

Model gModel;


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	Initialize init(0,0,1280,960);
	init.GLEWInit();

	Shader sh;
	vertexShader = sh.CreateVertexShader("VertexShader.glsl");		//VS 파일명
	fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");	//FS 파일명
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);

	gModel.ParseObj("Ghost.obj");
	gModel.PrintInfo();
	

	Renderer draw;
	glutDisplayFunc(draw.RenderScene);
	InputManager input;
	glutKeyboardFunc(input.Key);
	

	glutMainLoop();
}