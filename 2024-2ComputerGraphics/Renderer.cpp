#include "pch.h"
#include "Renderer.h"

GLuint Renderer::shaderProgramID = 0;
Camera gCamera;

Renderer::Renderer() {
	Shader sh;
	GLuint vertexShader = sh.CreateVertexShader("VertexShader.glsl");
	GLuint fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);
}

Renderer::~Renderer() {

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

	glUseProgram(shaderProgramID);

	//카메라 설정
	glm::mat4 view = gCamera.getViewMatrix();
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = gCamera.getProjectionMatrix(1280, 960);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//일단 큐브 하나
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 객체 회전
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

	Cube d;
	d.draw();	
	
	glm::mat4 playerMat = glm::mat4(1.0f);

	
	playerMat = glm::translate(playerMat, glm::vec3(
		gInput.GetPlayerXPos(),
		gInput.GetPlayerYPos(),
		gInput.GetPlayerZPos()
	));

	playerMat = glm::scale(playerMat, glm::vec3(0.3f, 0.3f, 0.3f));

	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(playerMat));


	//사용할때 주석 풀기
	gModel.BindBuffer();
	gModel.RenderPlayer();

	gCamera.SetCameraPos(
		gInput.GetPlayerXPos(),
		gInput.GetPlayerYPos(),
		gInput.GetPlayerZPos()
	);
	gCamera.SetCameraTarget(
		gInput.GetPlayerXPos(),
		gInput.GetPlayerYPos(),
		gInput.GetPlayerZPos()
	);

	// 뷰 행렬 계산 및 셰이더에 전달
	view = gCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glutSwapBuffers();
}
