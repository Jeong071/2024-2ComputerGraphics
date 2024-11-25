#include "pch.h"
#include "Draw.h"

Draw::Draw()
{
}

Draw::~Draw()
{
}

GLvoid Draw::DrawScene()
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
