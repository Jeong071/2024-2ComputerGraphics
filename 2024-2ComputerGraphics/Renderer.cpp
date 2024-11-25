#include "pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

GLvoid Renderer::RenderScene()
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT); 
	////////////////////////////////////////////
	//										  //
	//				Draw Code			      //
	//										  //
	////////////////////////////////////////////


	
	glutSwapBuffers();
}
