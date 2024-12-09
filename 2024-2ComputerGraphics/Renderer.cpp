#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "iostream"
using namespace std;

GLuint Renderer::shaderProgramID = 0;
Camera gCamera;
float playerAngle {0.0f};

Renderer::Renderer() {
	
}

Renderer::~Renderer() {

}

GLvoid Renderer::RenderScene()
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f); 
	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 버퍼 지우기 
	////////////////////////////////////////////
	//										  //
	//				Draw Code			      //
	//										  //
	////////////////////////////////////////////

	if (isVirtualMouse) {
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}

	glUseProgram(shaderProgramID);


	//카메라 설정
	glm::mat4 view = gCamera.getViewMatrix();
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = gCamera.getProjectionMatrix(1280, 960);

	
	Camera camera;
	view = camera.getViewMatrix();
	projection = camera.getProjectionMatrix(1280, 960);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	
	glm::vec3 cameraPos = glm::vec3(0.0f, 1.3f, 3.0f); 
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	bool lightOn = true;
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), lightOn);

	
	
	RenderStage2();
	RenderStage3();


  
	Cube d;
	//d.draw();	
	
	glm::mat4 playerMat = glm::mat4(1.0f);
	
	
	
	playerMat = glm::translate(playerMat, glm::vec3(
		gPlayer.GetPlayerXPos(),
		gPlayer.GetPlayerYPos(),
		gPlayer.GetPlayerZPos()
	));
	playerMat = rotate(playerMat, glm::radians(playerAngle), glm::vec3(0.0, rotateYAxis, 0.0));

	playerMat = glm::scale(playerMat, glm::vec3(0.06f, 0.06f, 0.06f));

	gCamera.SetCameraPos(
		gPlayer.GetPlayerXPos(),
		gPlayer.GetPlayerYPos(),
		gPlayer.GetPlayerZPos()
	);
	gCamera.SetCameraTarget(
		gPlayer.GetPlayerXPos(),
		gPlayer.GetPlayerYPos(),
		gPlayer.GetPlayerZPos()
	);


	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(playerMat));

	view = gCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	GLint uniformColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	glm::vec3 customColor(0.0f, 1.0f, 1.0f);
	glUniform3fv(uniformColorLocation, 1, glm::value_ptr(customColor));

	
	gModel.BindBuffer();
	gModel.RenderPlayer();
	RenderStage1();

	d.DeleteBuffer();
	gModel.ReleaseBuffer();
	glutSwapBuffers();
}

void Renderer::CreateShader()
{
	Shader sh;
	GLuint vertexShader = sh.CreateVertexShader("VertexShader.glsl");
	GLuint fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);
}

  



GLvoid Renderer::RenderStage1() {
	
	glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.0f);
	glUniform3fv(glGetUniformLocation(shaderProgramID, "objectColor"), 1, glm::value_ptr(objectColor));

	Cube cube1;
	cube1.position = glm::vec3(0.0f, 0.0f, 8.0f);
	cube1.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	cube1.rotationAngle = 0.0f;
	cube1.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube1.updateModelMatrix();
	cube1.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube1.modelMatrix));
	cube1.draw();
	cube1.DeleteBuffer();

	Cube cube2;
	cube2.position = glm::vec3(cube1.position.x + 0.0f, cube1.position.y + 0.0f, cube1.position.z-1.5f);
	cube2.scale = glm::vec3(0.5f, 1.0f, 2.0f);
	cube2.rotationAngle = 0.0f;
	cube2.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube2.updateModelMatrix();
	cube2.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube2.modelMatrix));
	cube2.draw();
	cube2.DeleteBuffer();

	Cube cube3;
	cube3.position = glm::vec3(cube2.position.x + 1.0f, cube2.position.y + 0.0f, cube2.position.z - 0.75f);
	cube3.scale = glm::vec3(0.5f, 1.0f, 2.0f);
	cube3.rotationAngle = 90.0f;
	cube3.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube3.updateModelMatrix();
	cube3.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube3.modelMatrix));
	cube3.draw();
	cube3.DeleteBuffer();

	Cube cube4;
	cube4.position = glm::vec3(cube3.position.x + 0.75f, cube3.position.y + 0.0f, cube3.position.z - 1.25f);
	cube4.scale = glm::vec3(0.5f, 1.0f, 2.0f);
	cube4.rotationAngle = 0.0f;
	cube4.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube4.updateModelMatrix();
	cube4.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube4.modelMatrix));
	cube4.draw();
	cube4.DeleteBuffer();

	Cube cube5;
	cube5.position = glm::vec3(cube4.position.x - 1.75f, cube4.position.y + 0.0f, cube4.position.z - 1.25f);
	cube5.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube5.rotationAngle = 90.0f;
	cube5.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube5.updateModelMatrix();
	cube5.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube5.modelMatrix));
	cube5.draw();
	cube5.DeleteBuffer();

	Cube cube6;
	cube6.position = glm::vec3(cube5.position.x - 1.75f, cube5.position.y + 0.0f, cube5.position.z - 1.25f);
	cube6.scale = glm::vec3(0.5f, 1.0f, 1.0f);
	cube6.rotationAngle = 0.0f;
	cube6.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube6.updateModelMatrix();
	cube6.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube6.modelMatrix));
	cube6.draw();
	cube6.DeleteBuffer();

	Cube cube7;
	cube7.position = glm::vec3(cube6.position.x + 0.75f, cube6.position.y + 0.0f, cube6.position.z - 1.25f);
	cube7.scale = glm::vec3(0.5f, 1.0f, 2.0f);
	cube7.rotationAngle = 90.0f;
	cube7.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube7.updateModelMatrix();
	cube7.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube7.modelMatrix));
	cube7.draw();
	cube7.DeleteBuffer();
	
	Cube cube8;
	cube8.position = glm::vec3(cube7.position.x + 1.0f, cube7.position.y + 0.0f, cube7.position.z - 2.75f);
	cube8.scale = glm::vec3(0.5f, 1.0f, 6.0f);
	cube8.rotationAngle = 0.0f;
	cube8.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube8.updateModelMatrix();
	cube8.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube8.modelMatrix));
	cube8.draw();
	cube8.DeleteBuffer();

	Cube cube9;
	cube9.position = glm::vec3(cube8.position.x + 1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube9.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube9.rotationAngle = 0.0f;
	cube9.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube9.updateModelMatrix();
	cube9.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube9.modelMatrix));
	cube9.draw();
	cube9.DeleteBuffer();

	Cube cube10;
	cube10.position = glm::vec3(cube8.position.x + -1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube10.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube10.rotationAngle = 0.0f;
	cube10.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube10.updateModelMatrix();
	cube10.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube10.modelMatrix));
	cube10.draw();
	cube10.DeleteBuffer();

	Cube cube11;
	cube11.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z + 1.75f);
	cube11.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube11.rotationAngle = 90.0f;
	cube11.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube11.updateModelMatrix();
	cube11.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube11.modelMatrix));
	cube11.draw();
	cube11.DeleteBuffer();

	Cube cube12;
	cube12.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z);
	cube12.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube12.rotationAngle = 90.0f;
	cube12.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube12.updateModelMatrix();
	cube12.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube12.modelMatrix));
	cube12.draw();
	cube12.DeleteBuffer();
	
	Cube cube13;
	cube13.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z - 1.75f);
	cube13.scale = glm::vec3(0.5f, 1.0f, 4.0f);
	cube13.rotationAngle = 90.0f;
	cube13.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube13.updateModelMatrix();
	cube13.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube13.modelMatrix));
	cube13.draw();
	cube13.DeleteBuffer();
}

GLvoid Renderer::RenderStage2() {
	
}

GLvoid Renderer::RenderStage3() {
	
}

