#include "pch.h"
#include "Renderer.h"

GLuint Renderer::shaderProgramID = 0;

Renderer::Renderer() {
	Shader sh;
	GLuint vertexShader = sh.CreateVertexShader("VertexShader.glsl");
	GLuint fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);
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

	glUseProgram(shaderProgramID);

	//카메라 설정
	Camera camera;
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = camera.getProjectionMatrix(1280, 960);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//일단 큐브 하나
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // 객체 회전
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

	Cube d;
	d.draw();

	glutSwapBuffers();
}
