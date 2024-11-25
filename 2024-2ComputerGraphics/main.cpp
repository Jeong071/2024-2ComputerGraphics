#include "pch.h"
#include "Renderer.h"
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	Initialize init(0,0,1000,1000);
	init.GLEWInit();

	Shader sh;
	vertexShader = sh.CreateVertexShader("VertexShader.glsl");		//VS ���ϸ�
	fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");	//FS ���ϸ�
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);

	Renderer draw;
	glutDisplayFunc(draw.RenderScene);

	glutMainLoop();
}