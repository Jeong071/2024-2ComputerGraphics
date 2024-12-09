#define STB_IMAGE_IMPLEMENTATION

#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "iostream"
#include "stb_image.h"
using namespace std;

GLuint Renderer::shaderProgramID = 0;
Camera gCamera;
float playerAngle {0.0f};

//텍스처 파일들 - 필요시 계속 추가
std::vector<std::string>textureFiles = { "floor_texture.jpg" };
std::vector<GLuint> loadTextures(const std::vector<std::string>& filenames) {
	std::vector<GLuint> textureIDs;
	for (const auto& filename : filenames) {
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 4);  // RGBA 포맷 강제
		if (data) {
			GLuint textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			textureIDs.push_back(textureID);  // 저장된 텍스처 ID 추가
			stbi_image_free(data);
		}
		else {
			std::cerr << "Failed to load texture: " << filename << std::endl;
		}
	}
	return textureIDs;
}

Renderer::Renderer() {
	
}

Renderer::~Renderer() {

}

GLvoid Renderer::RenderScene()
{
	glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화
	glDepthFunc(GL_LESS);     // 깊이 비교 함수 설정
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glm::vec3 lightPos(1.0f, 1.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	bool lightOn = true;
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), lightOn);
  
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
	RenderStage2();
	RenderStage3();

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
	std::vector<GLuint> textures = loadTextures(textureFiles);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	Cube cube1;
	cube1.position = glm::vec3(0.0f, 0.0f, 8.0f);
	cube1.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube1.rotationAngle = 0.0f;
	cube1.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube1.updateModelMatrix();
	cube1.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube1.modelMatrix));
	cube1.draw();
	cube1.DeleteBuffer();

	Cube cube2;
	cube2.position = glm::vec3(cube1.position.x + 0.0f, cube1.position.y + 0.0f, cube1.position.z-1.5f);
	cube2.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube2.rotationAngle = 0.0f;
	cube2.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube2.updateModelMatrix();
	cube2.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube2.modelMatrix));
	cube2.draw();
	cube2.DeleteBuffer();

	Cube cube3;
	cube3.position = glm::vec3(cube2.position.x + 1.0f, cube2.position.y + 0.0f, cube2.position.z - 0.75f);
	cube3.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube3.rotationAngle = 90.0f;
	cube3.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube3.updateModelMatrix();
	cube3.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube3.modelMatrix));
	cube3.draw();
	cube3.DeleteBuffer();

	Cube cube4;
	cube4.position = glm::vec3(cube3.position.x + 0.75f, cube3.position.y + 0.0f, cube3.position.z - 1.25f);
	cube4.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube4.rotationAngle = 0.0f;
	cube4.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube4.updateModelMatrix();
	cube4.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube4.modelMatrix));
	cube4.draw();
	cube4.DeleteBuffer();

	Cube cube5;
	cube5.position = glm::vec3(cube4.position.x - 1.75f, cube4.position.y + 0.0f, cube4.position.z - 1.25f);
	cube5.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube5.rotationAngle = 90.0f;
	cube5.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube5.updateModelMatrix();
	cube5.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube5.modelMatrix));
	cube5.draw();
	cube5.DeleteBuffer();

	Cube cube6;
	cube6.position = glm::vec3(cube5.position.x - 1.75f, cube5.position.y + 0.0f, cube5.position.z - 1.25f);
	cube6.scale = glm::vec3(0.5f, 0.2f, 1.0f);
	cube6.rotationAngle = 0.0f;
	cube6.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube6.updateModelMatrix();
	cube6.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube6.modelMatrix));
	cube6.draw();
	cube6.DeleteBuffer();

	Cube cube7;
	cube7.position = glm::vec3(cube6.position.x + 0.75f, cube6.position.y + 0.0f, cube6.position.z - 1.25f);
	cube7.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube7.rotationAngle = 90.0f;
	cube7.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube7.updateModelMatrix();
	cube7.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube7.modelMatrix));
	cube7.draw();
	cube7.DeleteBuffer();
	
	Cube cube8;
	cube8.position = glm::vec3(cube7.position.x + 1.0f, cube7.position.y + 0.0f, cube7.position.z - 2.75f);
	cube8.scale = glm::vec3(0.5f, 0.2f, 6.0f);
	cube8.rotationAngle = 0.0f;
	cube8.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube8.updateModelMatrix();
	cube8.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube8.modelMatrix));
	cube8.draw();
	cube8.DeleteBuffer();

	Cube cube9;
	cube9.position = glm::vec3(cube8.position.x + 1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube9.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube9.rotationAngle = 0.0f;
	cube9.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube9.updateModelMatrix();
	cube9.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube9.modelMatrix));
	cube9.draw();
	cube9.DeleteBuffer();

	Cube cube10;
	cube10.position = glm::vec3(cube8.position.x + -1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube10.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube10.rotationAngle = 0.0f;
	cube10.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube10.updateModelMatrix();
	cube10.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube10.modelMatrix));
	cube10.draw();
	cube10.DeleteBuffer();

	Cube cube11;
	cube11.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z + 1.75f);
	cube11.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube11.rotationAngle = 90.0f;
	cube11.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube11.updateModelMatrix();
	cube11.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube11.modelMatrix));
	cube11.draw();
	cube11.DeleteBuffer();

	Cube cube12;
	cube12.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z);
	cube12.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube12.rotationAngle = 90.0f;
	cube12.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube12.updateModelMatrix();
	cube12.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube12.modelMatrix));
	cube12.draw();
	cube12.DeleteBuffer();
	
	Cube cube13;
	cube13.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z - 1.75f);
	cube13.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube13.rotationAngle = 90.0f;
	cube13.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube13.updateModelMatrix();
	cube13.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube13.modelMatrix));
	cube13.draw();
	cube13.DeleteBuffer();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::RenderStage2() {
	std::vector<GLuint> textures = loadTextures(textureFiles);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	Cube cube14;
	cube14.position = glm::vec3(0.0f, 0.0f, -9.0f);
	cube14.scale = glm::vec3(8.0f, 0.2f, 8.0f);
	cube14.rotationAngle = 0.0f;
	cube14.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube14.updateModelMatrix();
	cube14.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube14.modelMatrix));
	cube14.draw();
	cube14.DeleteBuffer();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::RenderStage3() {
	std::vector<GLuint> textures = loadTextures(textureFiles);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	Cube cube15;
	cube15.position = glm::vec3(0.0f, 0.0f, -19.5f);
	cube15.scale = glm::vec3(0.5f, 0.2f, 12.0f);
	cube15.rotationAngle = 0.0f;
	cube15.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube15.updateModelMatrix();
	cube15.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube15.modelMatrix));
	cube15.draw();
	cube15.DeleteBuffer();

	Cube cube16;
	cube16.position = glm::vec3(cube15.position.x - 1.0f, cube15.position.y, cube15.position.z);
	cube16.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube16.rotationAngle = 0.0f;
	cube16.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube16.updateModelMatrix();
	cube16.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube16.modelMatrix));
	cube16.draw();
	cube16.DeleteBuffer();

	Cube cube17;
	cube17.position = glm::vec3(cube15.position.x - 2.0f, cube15.position.y, cube15.position.z);
	cube17.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube17.rotationAngle = 0.0f;
	cube17.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube17.updateModelMatrix();
	cube17.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube17.modelMatrix));
	cube17.draw();
	cube17.DeleteBuffer();

	Cube cube18;
	cube18.position = glm::vec3(cube15.position.x + 1.0f, cube15.position.y, cube15.position.z);
	cube18.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube18.rotationAngle = 0.0f;
	cube18.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube18.updateModelMatrix();
	cube18.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube18.modelMatrix));
	cube18.draw();
	cube18.DeleteBuffer();

	Cube cube19;
	cube19.position = glm::vec3(cube15.position.x + 2.0f, cube15.position.y, cube15.position.z);
	cube19.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube19.rotationAngle = 0.0f;
	cube19.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube19.updateModelMatrix();
	cube19.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube19.modelMatrix));
	cube19.draw();
	cube19.DeleteBuffer();

	//점프맵 시작
	Cube cube20;
	cube20.position = glm::vec3(cube15.position.x, cube15.position.y, cube15.position.z - 7.0f);
	cube20.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube20.rotationAngle = 0.0f;
	cube20.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube20.updateModelMatrix();
	cube20.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube20.modelMatrix));
	cube20.draw();
	cube20.DeleteBuffer();

	//점프맵 중간
	Cube cube21;
	cube21.position = glm::vec3(cube20.position.x, cube20.position.y, cube20.position.z - 12.0f);
	cube21.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube21.rotationAngle = 0.0f;
	cube21.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube21.updateModelMatrix();
	cube21.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube21.modelMatrix));
	cube21.draw();
	cube21.DeleteBuffer();

	//점프맵 끝
	Cube cube22;
	cube22.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 12.0f);
	cube22.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube22.rotationAngle = 0.0f;
	cube22.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube22.updateModelMatrix();
	cube22.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube22.modelMatrix));
	cube22.draw();
	cube22.DeleteBuffer();

	Cube cube23;
	cube23.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 10.0f);
	cube23.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube23.rotationAngle = 0.0f;
	cube23.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube23.updateModelMatrix();
	cube23.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube23.modelMatrix));
	cube23.draw();
	cube23.DeleteBuffer();

	Cube cube24;
	cube24.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 10.0f);
	cube24.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube24.rotationAngle = 0.0f;
	cube24.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube24.updateModelMatrix();
	cube24.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube24.modelMatrix));
	cube24.draw();
	cube24.DeleteBuffer();

	Cube cube25;
	cube25.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z + 8.0f);
	cube25.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube25.rotationAngle = 0.0f;
	cube25.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube25.updateModelMatrix();
	cube25.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube25.modelMatrix));
	cube25.draw();
	cube25.DeleteBuffer();

	Cube cube26;
	cube23.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 6.0f);
	cube23.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube23.rotationAngle = 0.0f;
	cube23.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube23.updateModelMatrix();
	cube23.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube23.modelMatrix));
	cube23.draw();
	cube23.DeleteBuffer();

	Cube cube27;
	cube27.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 6.0f);
	cube27.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube27.rotationAngle = 0.0f;
	cube27.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube27.updateModelMatrix();
	cube27.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube27.modelMatrix));
	cube27.draw();
	cube27.DeleteBuffer();

	Cube cube28;
	cube28.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z + 4.0f);
	cube28.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube28.rotationAngle = 0.0f;
	cube28.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube28.updateModelMatrix();
	cube28.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube28.modelMatrix));
	cube28.draw();
	cube28.DeleteBuffer();

	Cube cube29;
	cube29.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 2.0f);
	cube29.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube29.rotationAngle = 0.0f;
	cube29.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube29.updateModelMatrix();
	cube29.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube29.modelMatrix));
	cube29.draw();
	cube29.DeleteBuffer();

	Cube cube30;
	cube30.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 2.0f);
	cube30.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube30.rotationAngle = 0.0f;
	cube30.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube30.updateModelMatrix();
	cube30.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube30.modelMatrix));
	cube30.draw();
	cube30.DeleteBuffer();

	Cube cube31;
	cube31.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 2.0f);
	cube31.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube31.rotationAngle = 0.0f;
	cube31.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube31.updateModelMatrix();
	cube31.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube31.modelMatrix));
	cube31.draw();
	cube31.DeleteBuffer();

	Cube cube32;
	cube32.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 2.0f);
	cube32.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube32.rotationAngle = 0.0f;
	cube32.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube32.updateModelMatrix();
	cube32.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube32.modelMatrix));
	cube32.draw();
	cube32.DeleteBuffer();

	Cube cube33;
	cube33.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 4.0f);
	cube33.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube33.rotationAngle = 0.0f;
	cube33.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube33.updateModelMatrix();
	cube33.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube33.modelMatrix));
	cube33.draw();
	cube33.DeleteBuffer();

	Cube cube34;
	cube34.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 6.0f);
	cube34.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube34.rotationAngle = 0.0f;
	cube34.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube34.updateModelMatrix();
	cube34.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube34.modelMatrix));
	cube34.draw();
	cube34.DeleteBuffer();

	Cube cube35;
	cube35.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 6.0f);
	cube35.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube35.rotationAngle = 0.0f;
	cube35.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube35.updateModelMatrix();
	cube35.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube35.modelMatrix));
	cube35.draw();
	cube35.DeleteBuffer();

	Cube cube36;
	cube36.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 8.0f);
	cube36.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube36.rotationAngle = 0.0f;
	cube36.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube36.updateModelMatrix();
	cube36.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube36.modelMatrix));
	cube36.draw();
	cube36.DeleteBuffer();

	Cube cube37;
	cube37.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 10.0f);
	cube37.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube37.rotationAngle = 0.0f;
	cube37.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube37.updateModelMatrix();
	cube37.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube37.modelMatrix));
	cube37.draw();
	cube37.DeleteBuffer();

	Cube cube38;
	cube38.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 10.0f);
	cube38.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube38.rotationAngle = 0.0f;
	cube38.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube38.updateModelMatrix();
	cube38.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube38.modelMatrix));
	cube38.draw();
	cube38.DeleteBuffer();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);

}

