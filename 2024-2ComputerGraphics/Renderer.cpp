#define STB_IMAGE_IMPLEMENTATION

#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "iostream"
#include "stb_image.h"
using namespace std;

SceneType currentScene = MAIN_MENU;

std::vector<GLuint> Renderer::textureIDs{};
GLuint Renderer::shaderProgramID = 0;
Camera gCamera{};
Timer gTimer;
float playerAngle {0.0f};
std::vector<Cube> Renderer::cubes{};
std::vector<Cube> Renderer::objCubes{};
std::vector<Cube> Renderer::deathCubes{};

//몬스터 이동
bool isEnemyMoving = true;
float enemyArmRotationAngle = 0.0f;
float enemyLegRotationAngle = 0.0f;
float enemyArmRotationRate = 90.0f;
float enemyLegRotationRate = 90.0f;
//적 위치 벡터
glm::vec enemyPos = glm::vec3(0.0f, 0.6f, 7.0f);

//지형 애니메이션 변수
float lastTime = 0.0f;
float floorMoveSpeed = 1.1f;
float floorPosZ = 0.0f;

float wallMoveSpeeds[5] = { 1.1f, 1.1f, 1.1f, 1.1f, 1.1f }; 
float wallPosZ[5] = {0.0f};

//미사일 변수
float missileY = 5.0f;
float missileSpeed = 2.1f;

std::vector<float> missileYs = { 6.5f, 7.2f, 9.1f, 9.3f, 8.0f, 11.5f, 6.5f, 7.2f, 9.1f, 9.3f, 8.0f, 11.5f,  6.5f, 7.2f, 9.1f, 9.3f, 8.0f, 11.5f };
std::mt19937 rng(std::random_device{}());  // 난수 생성기 초기화
std::uniform_real_distribution<float> dist(8.0f, 15.0f);  // 3.0에서 13.0 사이의 값 생성

std::vector<GLuint> loadTextures(const std::vector<std::string>& filenames) {
	std::vector<GLuint> textureIDs;
	glActiveTexture(GL_TEXTURE0);

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
	
	if (currentScene == MAIN_MENU) {
		RenderMainScene();
	}
	

	else if (currentScene == GAME_PLAY) {
		RenderPlayScene();
	}

	

	glutSwapBuffers();
	gTimer.UpdateFPS();
}



void Renderer::InitializeTextures() {
	std::vector<std::string> textureFiles = {
		"floor_texture.jpg", "enemy_body.png", "enemy_head_face.png",
		"enemy_head.png", "enemy_nose.png", "enemy_arm.png", "enemy_underBody.png",
		"enemy_leg.png","front.jpg","back.jpg","left.jpg","right.jpg","bottom.jpg","top.jpg","Main.png",
	 "missile_target.png" , "lava.png"
	};
	textureIDs = loadTextures(textureFiles);
}

void Renderer::CreateShader()
{
	Shader sh;
	GLuint vertexShader = sh.CreateVertexShader("VertexShader.glsl");
	GLuint fragmentShader = sh.CreateFragmentShader("FragmentShader.glsl");
	shaderProgramID = sh.CreateShaderProgram(vertexShader, fragmentShader);
}

GLvoid Renderer::RenderEnemy() { 
	Cube enemyBody;
	enemyBody.position = glm::vec3(enemyPos.x, enemyPos.y, enemyPos.z);
	enemyBody.scale = glm::vec3(0.4f, 0.2f, 0.2f);
	enemyBody.rotationAngle = 0.0f;
	enemyBody.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	enemyBody.updateModelMatrix();
	enemyBody.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(enemyBody.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[1]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	enemyBody.draw(36, 0);
	enemyBody.DeleteBuffer();

	//충돌판정용 큐브
	Cube enemy;
	enemy.position = glm::vec3(enemyPos.x, enemyPos.y - 0.1f, enemyPos.z);
	enemy.scale = glm::vec3(0.5f, 1.0f, 0.4f);
	enemy.rotationAngle = 0.0f;
	enemy.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	enemy.updateModelMatrix();
	enemy.updateBounds();
	enemy.DeleteBuffer();
	deathCubes.emplace_back(enemy);

	Cube enemyHead;
	enemyHead.position = glm::vec3(enemyBody.position.x + 0.0f, enemyBody.position.y + 0.185, enemyBody.position.z - 0.05f);
	enemyHead.scale = glm::vec3(0.15f, 0.17f, 0.15f);
	enemyHead.rotationAngle = 0.0f;
	enemyHead.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	enemyHead.updateModelMatrix();
	enemyHead.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(enemyHead.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[2]);
	enemyBody.draw(6, 0);
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[3]);
	for (int i = 1; i < 6; ++i) {
		enemyBody.draw(6, (void*)(i * 6 * sizeof(GLuint)));
	}
	enemyHead.DeleteBuffer();

	Cube enemyNose;
	enemyNose.position = glm::vec3(enemyBody.position.x + 0.0f, enemyBody.position.y + 0.11, enemyBody.position.z - 0.14f);
	enemyNose.scale = glm::vec3(0.04f, 0.06f, 0.04f);
	enemyNose.rotationAngle = 0.0f;
	enemyNose.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	enemyNose.updateModelMatrix();
	enemyNose.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(enemyNose.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[4]);
	enemyNose.draw(36, 0);
	enemyNose.DeleteBuffer();

	Cube enemyLeftArm;
	glm::mat4 leftArmModelMatrix = glm::mat4(1.0f);
	leftArmModelMatrix = glm::translate(leftArmModelMatrix, enemyBody.position);
	leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(0.24f, 0.1f, 0.0f));
	leftArmModelMatrix = glm::rotate(leftArmModelMatrix, glm::radians(enemyArmRotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
	leftArmModelMatrix = glm::translate(leftArmModelMatrix, glm::vec3(0.0f, - 0.23f, 0.0f));
	leftArmModelMatrix = glm::scale(leftArmModelMatrix, glm::vec3(0.08f, 0.48f, 0.12f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(leftArmModelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[7]);
	enemyLeftArm.draw(36, 0);
	enemyLeftArm.DeleteBuffer();

	Cube enemyRightArm;
	glm::mat4 rightArmModelMatrix = glm::mat4(1.0f);
	rightArmModelMatrix = glm::translate(rightArmModelMatrix, enemyBody.position);
	rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(-0.24f, 0.1f, 0.0f));
	rightArmModelMatrix = glm::rotate(rightArmModelMatrix, glm::radians(enemyArmRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	rightArmModelMatrix = glm::translate(rightArmModelMatrix, glm::vec3(0.0f, -0.23f, 0.0f));
	rightArmModelMatrix = glm::scale(rightArmModelMatrix, glm::vec3(0.08f, 0.48f, 0.12f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(rightArmModelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[7]);
	enemyLeftArm.draw(36, 0);
	enemyLeftArm.DeleteBuffer();

	Cube enemyUnderBody;
	enemyUnderBody.position = glm::vec3(enemyBody.position.x, enemyBody.position.y - 0.13, enemyBody.position.z);
	enemyUnderBody.scale = glm::vec3(0.2f, 0.1f, 0.1f);
	enemyUnderBody.rotationAngle = 0.0f;
	enemyUnderBody.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	enemyUnderBody.updateModelMatrix();
	enemyUnderBody.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(enemyUnderBody.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[6]);
	enemyLeftArm.draw(36, 0);
	enemyUnderBody.DeleteBuffer();

	Cube enemyLeftLeg;
	glm::mat4 leftLegModelMatrix = glm::mat4(1.0f);
	leftLegModelMatrix = glm::translate(leftLegModelMatrix, enemyUnderBody.position);
	leftLegModelMatrix = glm::rotate(leftLegModelMatrix, glm::radians(enemyLegRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	leftLegModelMatrix = glm::translate(leftLegModelMatrix, glm::vec3(0.08f, -0.2f, 0.0f));
	leftLegModelMatrix = glm::scale(leftLegModelMatrix, glm::vec3(0.12f, 0.34f, 0.12f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(leftLegModelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[7]);
	enemyLeftArm.draw(36, 0);
	enemyLeftLeg.DeleteBuffer();

	Cube enemyRightLeg;
	glm::mat4 rightLegModelMatrix = glm::mat4(1.0f);
	rightLegModelMatrix = glm::translate(rightLegModelMatrix, enemyUnderBody.position);
	rightLegModelMatrix = glm::rotate(rightLegModelMatrix, glm::radians(enemyLegRotationAngle), glm::vec3(-1.0f, 0.0f, 0.0f));
	rightLegModelMatrix = glm::translate(rightLegModelMatrix, glm::vec3(-0.08f, -0.2f, 0.0f));
	rightLegModelMatrix = glm::scale(rightLegModelMatrix, glm::vec3(0.12f, 0.34f, 0.12f));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(rightLegModelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[7]);
	enemyLeftArm.draw(36, 0);
	enemyLeftLeg.DeleteBuffer();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::RenderStage1() {
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[0]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);


	Cube temp;
	temp.position = glm::vec3(0.0f, 0.0f, 6.0f);
	temp.scale = glm::vec3(0.2f, 1.0f, 0.2f);
	temp.rotationAngle = 0.0f;
	temp.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	temp.updateModelMatrix();
	temp.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(temp.modelMatrix));
	temp.draw(36, 0);
	temp.DeleteBuffer();
	objCubes.emplace_back(temp);

	Cube cube1;
	cube1.position = glm::vec3(0.0f, 0.0f, 8.0f);
	cube1.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube1.rotationAngle = 0.0f;
	cube1.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube1.updateModelMatrix();
	cube1.updateBounds(); 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube1.modelMatrix));
	cube1.draw(36, 0);
	cube1.DeleteBuffer();
	cubes.emplace_back(cube1);
	
	Cube cube2;
	cube2.position = glm::vec3(cube1.position.x + 0.0f, cube1.position.y + 0.0f, cube1.position.z-1.5f);
	cube2.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube2.rotationAngle = 0.0f;
	cube2.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube2.updateModelMatrix();
	cube2.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube2.modelMatrix));
	cube2.draw(36, 0);
	cube2.DeleteBuffer();
	cubes.emplace_back(cube2);
	
	
	Cube cube3;
	cube3.position = glm::vec3(cube2.position.x + 1.0f, cube2.position.y + 0.0f, cube2.position.z - 0.75f);
	cube3.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube3.rotationAngle = 90.0f;
	cube3.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube3.updateModelMatrix();
	cube3.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube3.modelMatrix));
	cube3.draw(36, 0);
	cube3.DeleteBuffer();
	cubes.emplace_back(cube3);

	Cube cube4;
	cube4.position = glm::vec3(cube3.position.x + 0.75f, cube3.position.y + 0.0f, cube3.position.z - 1.25f);
	cube4.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube4.rotationAngle = 0.0f;
	cube4.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube4.updateModelMatrix();
	cube4.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube4.modelMatrix));
	cube4.draw(36, 0);
	cube4.DeleteBuffer();
	cubes.emplace_back(cube4);

	Cube cube5;
	cube5.position = glm::vec3(cube4.position.x - 1.75f, cube4.position.y + 0.0f, cube4.position.z - 1.25f);
	cube5.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube5.rotationAngle = 90.0f;
	cube5.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube5.updateModelMatrix();
	cube5.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube5.modelMatrix));
	cube5.draw(36, 0);
	cube5.DeleteBuffer();
	cubes.emplace_back(cube5);

	Cube cube6;
	cube6.position = glm::vec3(cube5.position.x - 1.75f, cube5.position.y + 0.0f, cube5.position.z - 1.25f);
	cube6.scale = glm::vec3(0.5f, 0.2f, 1.0f);
	cube6.rotationAngle = 0.0f;
	cube6.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube6.updateModelMatrix();
	cube6.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube6.modelMatrix));
	cube6.draw(36, 0);
	cube6.DeleteBuffer();
	cubes.emplace_back(cube6);

	Cube cube7;
	cube7.position = glm::vec3(cube6.position.x + 0.75f, cube6.position.y + 0.0f, cube6.position.z - 1.25f);
	cube7.scale = glm::vec3(0.5f, 0.2f, 2.0f);
	cube7.rotationAngle = 90.0f;
	cube7.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube7.updateModelMatrix();
	cube7.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube7.modelMatrix));
	cube7.draw(36, 0);
	cube7.DeleteBuffer();
	cubes.emplace_back(cube7);
	
	Cube cube8;
	cube8.position = glm::vec3(cube7.position.x + 1.0f, cube7.position.y + 0.0f, cube7.position.z - 2.75f);
	cube8.scale = glm::vec3(0.5f, 0.2f, 6.0f);
	cube8.rotationAngle = 0.0f;
	cube8.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube8.updateModelMatrix();
	cube8.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube8.modelMatrix));
	cube8.draw(36, 0);
	cube8.DeleteBuffer();
	cubes.emplace_back(cube8);

	Cube cube9;
	cube9.position = glm::vec3(cube8.position.x + 1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube9.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube9.rotationAngle = 0.0f;
	cube9.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube9.updateModelMatrix();
	cube9.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube9.modelMatrix));
	cube9.draw(36, 0);
	cube9.DeleteBuffer();
	cubes.emplace_back(cube9);

	Cube cube10;
	cube10.position = glm::vec3(cube8.position.x + -1.75f, cube8.position.y + 0.0f, cube8.position.z);
	cube10.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube10.rotationAngle = 0.0f;
	cube10.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube10.updateModelMatrix();
	cube10.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube10.modelMatrix));
	cube10.draw(36, 0);
	cube10.DeleteBuffer();
	cubes.emplace_back(cube10);

	Cube cube11;
	cube11.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z + 1.75f);
	cube11.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube11.rotationAngle = 90.0f;
	cube11.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube11.updateModelMatrix();
	cube11.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube11.modelMatrix));
	cube11.draw(36, 0);
	cube11.DeleteBuffer();
	cubes.emplace_back(cube11);

	Cube cube12;
	cube12.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z);
	cube12.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube12.rotationAngle = 90.0f;
	cube12.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube12.updateModelMatrix();
	cube12.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube12.modelMatrix));
	cube12.draw(36, 0);
	cube12.DeleteBuffer();
	cubes.emplace_back(cube12);
	
	Cube cube13;
	cube13.position = glm::vec3(cube8.position.x, cube8.position.y + 0.0f, cube8.position.z - 1.75f);
	cube13.scale = glm::vec3(0.5f, 0.2f, 4.0f);
	cube13.rotationAngle = 90.0f;
	cube13.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube13.updateModelMatrix();
	cube13.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube13.modelMatrix));
	cube13.draw(36, 0);
	cube13.DeleteBuffer();
	cubes.emplace_back(cube13);

	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::RenderStage2() {
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[0]);
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
	cube14.draw(36, 0);
	cube14.DeleteBuffer();
	cubes.emplace_back(cube14);
	
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::RenderStage3() {
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[0]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	Cube cube15;
	cube15.position = glm::vec3(0.0f, 0.0f, -19.5f);
	cube15.scale = glm::vec3(0.5f, 0.2f, 13.0f);
	cube15.rotationAngle = 0.0f;
	cube15.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube15.updateModelMatrix();
	cube15.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube15.modelMatrix));
	cube15.draw(36, 0);
	cube15.DeleteBuffer();
	cubes.emplace_back(cube15);

	Cube cube16;
	cube16.position = glm::vec3(cube15.position.x - 0.7f, cube15.position.y, cube15.position.z);
	cube16.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube16.rotationAngle = 0.0f;
	cube16.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube16.updateModelMatrix();
	cube16.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube16.modelMatrix));
	cube16.draw(36, 0);
	cube16.DeleteBuffer();
	cubes.emplace_back(cube16);

	Cube cube17;
	cube17.position = glm::vec3(cube15.position.x - 1.4f, cube15.position.y, cube15.position.z);
	cube17.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube17.rotationAngle = 0.0f;
	cube17.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube17.updateModelMatrix();
	cube17.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube17.modelMatrix));
	cube17.draw(36, 0);
	cube17.DeleteBuffer();
	cubes.emplace_back(cube17);

	Cube cube18;
	cube18.position = glm::vec3(cube15.position.x + 0.7f, cube15.position.y, cube15.position.z);
	cube18.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube18.rotationAngle = 0.0f;
	cube18.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube18.updateModelMatrix();
	cube18.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube18.modelMatrix));
	cube18.draw(36, 0);
	cube18.DeleteBuffer();
	cubes.emplace_back(cube18);

	Cube cube19;
	cube19.position = glm::vec3(cube15.position.x + 1.4f, cube15.position.y, cube15.position.z);
	cube19.scale = glm::vec3(0.5f, 0.2f, 10.0f);
	cube19.rotationAngle = 0.0f;
	cube19.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube19.updateModelMatrix();
	cube19.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube19.modelMatrix));
	cube19.draw(36, 0);
	cube19.DeleteBuffer();
	cubes.emplace_back(cube19);

	//점프맵 시작
	Cube cube20;
	cube20.position = glm::vec3(cube15.position.x, cube15.position.y, cube15.position.z - 7.0f);
	cube20.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube20.rotationAngle = 0.0f;
	cube20.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube20.updateModelMatrix();
	cube20.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube20.modelMatrix));
	cube20.draw(36, 0);
	cube20.DeleteBuffer(); 
	cubes.emplace_back(cube20);


	//점프맵 중간
	Cube cube21;
	cube21.position = glm::vec3(cube20.position.x, cube20.position.y, cube20.position.z - 12.0f);
	cube21.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube21.rotationAngle = 0.0f;
	cube21.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube21.updateModelMatrix();
	cube21.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube21.modelMatrix));
	cube21.draw(36, 0);
	cube21.DeleteBuffer();
	cubes.emplace_back(cube21);

	//점프맵 끝
	Cube cube22;
	cube22.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 12.0f);
	cube22.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube22.rotationAngle = 0.0f;
	cube22.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube22.updateModelMatrix();
	cube22.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube22.modelMatrix));
	cube22.draw(36, 0);
	cube22.DeleteBuffer();
	cubes.emplace_back(cube22);

	Cube cube23;
	cube23.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 10.0f);
	cube23.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube23.rotationAngle = 0.0f;
	cube23.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube23.updateModelMatrix();
	cube23.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube23.modelMatrix));
	cube23.draw(36, 0);
	cube23.DeleteBuffer();
	cubes.emplace_back(cube23);

	Cube cube24;
	cube24.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 10.0f);
	cube24.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube24.rotationAngle = 0.0f;
	cube24.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube24.updateModelMatrix();
	cube24.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube24.modelMatrix));
	cube24.draw(36, 0);
	cube24.DeleteBuffer();
	cubes.emplace_back(cube24);

	Cube cube25;
	cube25.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z + 8.0f);
	cube25.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube25.rotationAngle = 0.0f;
	cube25.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube25.updateModelMatrix();
	cube25.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube25.modelMatrix));
	cube25.draw(36, 0);
	cube25.DeleteBuffer(); 
	cubes.emplace_back(cube25);

	Cube cube26;
	cube23.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 6.0f);
	cube23.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube23.rotationAngle = 0.0f;
	cube23.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube23.updateModelMatrix();
	cube23.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube23.modelMatrix));
	cube23.draw(36, 0);
	cube23.DeleteBuffer();
	cubes.emplace_back(cube26);

	Cube cube27;
	cube27.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 6.0f);
	cube27.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube27.rotationAngle = 0.0f;
	cube27.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube27.updateModelMatrix();
	cube27.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube27.modelMatrix));
	cube27.draw(36, 0);
	cube27.DeleteBuffer();
	cubes.emplace_back(cube27);

	Cube cube28;
	cube28.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z + 4.0f);
	cube28.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube28.rotationAngle = 0.0f;
	cube28.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube28.updateModelMatrix();
	cube28.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube28.modelMatrix));
	cube28.draw(36, 0);
	cube28.DeleteBuffer();
	cubes.emplace_back(cube28);

	Cube cube29;
	cube29.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z + 2.0f);
	cube29.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube29.rotationAngle = 0.0f;
	cube29.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube29.updateModelMatrix();
	cube29.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube29.modelMatrix));
	cube29.draw(36, 0);
	cube29.DeleteBuffer();
	cubes.emplace_back(cube29);

	Cube cube30;
	cube30.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z + 2.0f);
	cube30.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube30.rotationAngle = 0.0f;
	cube30.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube30.updateModelMatrix();
	cube30.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube30.modelMatrix));
	cube30.draw(36, 0);
	cube30.DeleteBuffer();
	cubes.emplace_back(cube30);

	Cube cube31;
	cube31.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 2.0f);
	cube31.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube31.rotationAngle = 0.0f;
	cube31.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube31.updateModelMatrix();
	cube31.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube31.modelMatrix));
	cube31.draw(36, 0);
	cube31.DeleteBuffer();
	cubes.emplace_back(cube31);

	Cube cube32;
	cube32.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 2.0f);
	cube32.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube32.rotationAngle = 0.0f;
	cube32.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube32.updateModelMatrix();
	cube32.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube32.modelMatrix));
	cube32.draw(36, 0);
	cube32.DeleteBuffer();
	cubes.emplace_back(cube32);

	Cube cube33;
	cube33.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 4.0f);
	cube33.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube33.rotationAngle = 0.0f;
	cube33.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube33.updateModelMatrix();
	cube33.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube33.modelMatrix));
	cube33.draw(36, 0);
	cube33.DeleteBuffer();
	cubes.emplace_back(cube33);

	Cube cube34;
	cube34.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 6.0f);
	cube34.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube34.rotationAngle = 0.0f;
	cube34.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube34.updateModelMatrix();
	cube34.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube34.modelMatrix));
	cube34.draw(36, 0);
	cube34.DeleteBuffer();
	cubes.emplace_back(cube34);

	Cube cube35;
	cube35.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 6.0f);
	cube35.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube35.rotationAngle = 0.0f;
	cube35.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube35.updateModelMatrix();
	cube35.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube35.modelMatrix));
	cube35.draw(36, 0);
	cube35.DeleteBuffer();
	cubes.emplace_back(cube35);


	Cube cube36;
	cube36.position = glm::vec3(cube21.position.x, cube21.position.y, cube21.position.z - 8.0f);
	cube36.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube36.rotationAngle = 0.0f;
	cube36.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube36.updateModelMatrix();
	cube36.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube36.modelMatrix));
	cube36.draw(36, 0);
	cube36.DeleteBuffer();
	cubes.emplace_back(cube36);

	Cube cube37;
	cube37.position = glm::vec3(cube21.position.x + 2.0f, cube21.position.y, cube21.position.z - 10.0f);
	cube37.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube37.rotationAngle = 0.0f;
	cube37.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube37.updateModelMatrix();
	cube37.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube37.modelMatrix));
	cube37.draw(36, 0);
	cube37.DeleteBuffer();
	cubes.emplace_back(cube37);

	Cube cube38;
	cube38.position = glm::vec3(cube21.position.x - 2.0f, cube21.position.y, cube21.position.z - 10.0f);
	cube38.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube38.rotationAngle = 0.0f;
	cube38.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube38.updateModelMatrix();
	cube38.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube38.modelMatrix));
	cube38.draw(36, 0);
	cube38.DeleteBuffer();
	cubes.emplace_back(cube38);

	//이동 발판
	Cube cube39;
	cube39.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z + 10.0f + floorPosZ);
	cube39.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube39.rotationAngle = 0.0f;
	cube39.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube39.updateModelMatrix();
	cube39.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube39.modelMatrix));
	cube39.draw(36, 0);
	cube39.DeleteBuffer();

	Cube cube40;
	cube40.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z + 10.0f + floorPosZ);
	cube40.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube40.rotationAngle = 0.0f;
	cube40.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube40.updateModelMatrix();
	cube40.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube40.modelMatrix));
	cube40.draw(36, 0);
	cube40.DeleteBuffer();

	Cube cube41;
	cube41.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z + 6.0f + floorPosZ);
	cube41.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube41.rotationAngle = 0.0f;
	cube41.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube41.updateModelMatrix();
	cube41.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube41.modelMatrix));
	cube41.draw(36, 0);
	cube41.DeleteBuffer();

	Cube cube42;
	cube42.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z + 6.0f + floorPosZ);
	cube42.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube42.rotationAngle = 0.0f;
	cube42.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube42.updateModelMatrix();
	cube42.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube42.modelMatrix));
	cube42.draw(36, 0);
	cube42.DeleteBuffer();

	Cube cube43;
	cube43.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z + 2.0f + floorPosZ);
	cube43.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube43.rotationAngle = 0.0f;
	cube43.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube43.updateModelMatrix();
	cube43.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube43.modelMatrix));
	cube43.draw(36, 0);
	cube43.DeleteBuffer();

	Cube cube44;
	cube44.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z + 2.0f + floorPosZ);
	cube44.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube44.rotationAngle = 0.0f;
	cube44.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube44.updateModelMatrix();
	cube44.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube44.modelMatrix));
	cube44.draw(36, 0);
	cube44.DeleteBuffer();

	Cube cube45;
	cube45.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z - 2.0f + floorPosZ);
	cube45.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube45.rotationAngle = 0.0f;
	cube45.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube45.updateModelMatrix();
	cube45.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube45.modelMatrix));
	cube45.draw(36, 0);
	cube45.DeleteBuffer();

	Cube cube46;
	cube46.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z - 2.0f + floorPosZ);
	cube46.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube46.rotationAngle = 0.0f;
	cube46.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube46.updateModelMatrix();
	cube46.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube46.modelMatrix));
	cube46.draw(36, 0);
	cube46.DeleteBuffer();

	Cube cube47;
	cube47.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z - 6.0f + floorPosZ);
	cube47.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube47.rotationAngle = 0.0f;
	cube47.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube47.updateModelMatrix();
	cube47.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube47.modelMatrix));
	cube47.draw(36, 0);
	cube47.DeleteBuffer();

	Cube cube48;
	cube48.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z - 6.0f + floorPosZ);
	cube48.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube48.rotationAngle = 0.0f;
	cube48.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube48.updateModelMatrix();
	cube48.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube48.modelMatrix));
	cube48.draw(36, 0);
	cube48.DeleteBuffer();

	Cube cube49;
	cube49.position = glm::vec3(cube21.position.x - 1.0f, cube21.position.y, cube21.position.z - 10.0f + floorPosZ);
	cube49.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube49.rotationAngle = 0.0f;
	cube49.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube49.updateModelMatrix();
	cube49.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube49.modelMatrix));
	cube49.draw(36, 0);
	cube49.DeleteBuffer();

	Cube cube50;
	cube50.position = glm::vec3(cube21.position.x + 1.0f, cube21.position.y, cube21.position.z - 10.0f + floorPosZ);
	cube50.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube50.rotationAngle = 0.0f;
	cube50.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube50.updateModelMatrix();
	cube50.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube50.modelMatrix));
	cube50.draw(36, 0);
	cube50.DeleteBuffer();

	//골인 지점
	Cube cube51;
	cube51.position = glm::vec3(0.0f, 0.0f, -51.5);
	cube51.scale = glm::vec3(1.0f, 0.2f, 1.0f);
	cube51.rotationAngle = 0.0f;
	cube51.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	cube51.updateModelMatrix();
	cube51.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(cube51.modelMatrix));
	cube51.draw(36, 0);
	cube51.DeleteBuffer();

	


	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);

}

void Renderer::RenderSkyBox()
{
	
	glDepthFunc(GL_LEQUAL);


	glFrontFace(GL_CW);
	Cube skyBox;
	skyBox.position = glm::vec3(0.0f, 0.0f, 0.0f);
	skyBox.scale = glm::vec3(150.0f, 150.0f, 150.0f);
	skyBox.rotationAngle = 0.0f;
	skyBox.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);

	

	skyBox.updateModelMatrix();
	
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(skyBox.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[8]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, 0);//정면

	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[9]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, (void*)(6 * sizeof(unsigned int))); //후면

	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[10]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, (void*)(12 * sizeof(unsigned int))); //좌측 문제

	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[11]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, (void*)(18 * sizeof(unsigned int))); //우측

	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[12]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, (void*)(24 * sizeof(unsigned int))); //하단

	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[13]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	skyBox.draw(6, (void*)(30 * sizeof(unsigned int))); //상단

	glFrontFace(GL_CCW);

	skyBox.DeleteBuffer();

	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);

	glDepthFunc(GL_LESS);

}




void Renderer::ProcessCollision()
{
	bool collided = false;
	for (Cube& b : cubes) {
	if (CheckCollision(b)) {
			collided = true;

		}
	}

	if (!gPlayer.GetIsJumping()) {
		gPlayer.SetIsFalling(!collided);
	}
}

bool Renderer::CheckCollision(Cube& b)
{
	
	
	return (gPlayer.GetMaxPoint().x > b.GetCubeMinAABB().x && gPlayer.GetMinPoint().x < b.GetCubeMaxAABB().x) &&
		(gPlayer.GetMaxPoint().y > b.GetCubeMinAABB().y && gPlayer.GetMinPoint().y < b.GetCubeMaxAABB().y) &&
		(gPlayer.GetMaxPoint().z > b.GetCubeMinAABB().z && gPlayer.GetMinPoint().z < b.GetCubeMaxAABB().z);
}



GLvoid Renderer::RenderMainScene()
{
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
	glm::vec3 lightPos(0.0f, 100.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	bool lightOn = true;
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), lightOn);
	
	
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[14]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambient"), 1.0);
	
	
	Cube main;
	main.position = glm::vec3(0.0f, 0.0f, 5.0f);
	main.scale = glm::vec3(6.6f, 5.0f, 1.0f);
	main.rotationAngle = 0.0f;
	main.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	main.updateModelMatrix();
	main.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(main.modelMatrix));
	main.draw(6, 0);
	main.DeleteBuffer();

	
	
	glBindVertexArray(0);
}

GLvoid Renderer::RenderPlayScene()
{
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
	glm::vec3 lightPos(0.0f, 100.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	bool lightOn = true;
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightPos"), 1, glm::value_ptr(lightPos));
	glUniform3fv(glGetUniformLocation(shaderProgramID, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform1i(glGetUniformLocation(shaderProgramID, "lightOn"), lightOn);
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambient"), 0.5);



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
	playerMat = glm::translate(playerMat, glm::vec3(0.0f, 1.3f, 0.0f));

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
	glm::vec3 customColor(1.0f, 1.0f, 1.0f);
	glUniform3fv(uniformColorLocation, 1, glm::value_ptr(customColor));


	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);


	gModel.BindBuffer();
	gModel.RenderPlayer();

	//�� ����
	RenderEnemy();

	//�������� ����
	RenderStage1();
	RenderStage2(); 	//2�������� �� �� (-4, -0.1, -13) , (4, 0.1, -5)
	RenderStage3();
	ProcessCollision();
	RenderObstacle();

	update();



	glm::mat4 viewNoRotation = gCamera.getViewMatrix();
	projection = gCamera.getProjectionMatrix(WIDTH, HEIGHT);
	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewNoRotation));
	// ���̴� ���
	glUseProgram(shaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(viewNoTranslation));

	RenderSkyBox();


	
}

GLvoid Renderer::RenderMissile(float startX, float startY, float startZ) {
	Cube Target;
	Target.position = glm::vec3(startX, 0.251f, startZ);
	Target.scale = glm::vec3(1.0f, 1.0f, 0.3f);
	Target.rotationAngle = -90.0f;
	Target.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	Target.updateModelMatrix();
	Target.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(Target.modelMatrix));
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[15]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);
	Target.draw(6, 0);
	Target.DeleteBuffer();
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);

	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.8f, 0.8f, 0.8f);
	Cube Missile;
	Missile.position = glm::vec3(startX, startY, startZ);
	Missile.scale = glm::vec3(0.1f, 0.5f, 0.1f);
	Missile.rotationAngle = 0.0f;
	Missile.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	Missile.updateModelMatrix();
	Missile.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(Missile.modelMatrix));
	Missile.draw(36, 0);
	Missile.DeleteBuffer();

	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.9f, 0.9f, 0.9f);
	Cube MissileHead;
	MissileHead.position = glm::vec3(Missile.position.x + 0.0f, Missile.position.y - 0.28f, Missile.position.z);
	MissileHead.scale = glm::vec3(0.08f, 0.08f, 0.08f);
	MissileHead.rotationAngle = 0.0f;
	MissileHead.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	MissileHead.updateModelMatrix();
	MissileHead.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(MissileHead.modelMatrix));
	MissileHead.draw(36, 0);
	MissileHead.DeleteBuffer();

	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.4f, 0.4f, 0.4f);
	Cube MissileFin;
	MissileFin.position = glm::vec3(Missile.position.x + 0.065f, Missile.position.y + 0.28f, Missile.position.z);
	MissileFin.scale = glm::vec3(0.03f, 0.2f, 0.03f);
	MissileFin.rotationAngle = -10.0f;
	MissileFin.rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	MissileFin.updateModelMatrix();
	MissileFin.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(MissileFin.modelMatrix));
	MissileFin.draw(36, 0);
	MissileFin.DeleteBuffer();

	Cube MissileFin2;
	MissileFin2.position = glm::vec3(Missile.position.x - 0.065f, Missile.position.y + 0.28f, Missile.position.z);
	MissileFin2.scale = glm::vec3(0.03f, 0.2f, 0.03f);
	MissileFin2.rotationAngle = 10.0f;
	MissileFin2.rotationAxis = glm::vec3(0.0f, 1.0f, 1.0f);
	MissileFin2.updateModelMatrix();
	MissileFin2.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(MissileFin2.modelMatrix));
	MissileFin2.draw(36, 0);
	MissileFin2.DeleteBuffer();

	Cube MissileFin3;
	MissileFin3.position = glm::vec3(Missile.position.x, Missile.position.y + 0.28f, Missile.position.z + 0.065f);
	MissileFin3.scale = glm::vec3(0.03f, 0.2f, 0.03f);
	MissileFin3.rotationAngle = 10.0f;
	MissileFin3.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	MissileFin3.updateModelMatrix();
	MissileFin3.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(MissileFin3.modelMatrix));
	MissileFin3.draw(36, 0);
	MissileFin3.DeleteBuffer();

	Cube MissileFin4;
	MissileFin4.position = glm::vec3(Missile.position.x, Missile.position.y + 0.28f, Missile.position.z - 0.065f);
	MissileFin4.scale = glm::vec3(0.03f, 0.2f, 0.03f);
	MissileFin4.rotationAngle = -10.0f;
	MissileFin4.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	MissileFin4.updateModelMatrix();
	MissileFin4.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(MissileFin4.modelMatrix));
	MissileFin4.draw(36, 0);
	MissileFin4.DeleteBuffer();

	Cube Collision; // �浹üũ�� ť�� (�񰡽�)
	Collision.position = glm::vec3(Missile.position.x, Missile.position.y, Missile.position.z);
	Collision.scale = glm::vec3(0.5f, 1.0f, 0.5f);
	Collision.rotationAngle = 0.0f;
	Collision.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	Collision.updateModelMatrix();
	Collision.updateBounds();

	deathCubes.emplace_back(Collision);
}
GLvoid Renderer::RenderObstacle() {
	glBindTexture(GL_TEXTURE_2D, Renderer::textureIDs[16]);
	glUniform1i(glGetUniformLocation(shaderProgramID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 1);

	Cube stage3_movingWall1;
	stage3_movingWall1.position = glm::vec3(0.0f, 0.7f, -19.5f + wallPosZ[0]);
	stage3_movingWall1.scale = glm::vec3(0.5f, 1.2f, 0.2f);
	stage3_movingWall1.rotationAngle = 0.0f;
	stage3_movingWall1.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	stage3_movingWall1.updateModelMatrix();
	stage3_movingWall1.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(stage3_movingWall1.modelMatrix));
	stage3_movingWall1.draw(36, 0);
	stage3_movingWall1.DeleteBuffer();

	Cube stage3_movingWall2;
	stage3_movingWall2.position = glm::vec3(0.7f, 0.7f, -17.5f + wallPosZ[1]);
	stage3_movingWall2.scale = glm::vec3(0.5f, 1.2f, 0.2f);
	stage3_movingWall2.rotationAngle = 0.0f;
	stage3_movingWall2.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	stage3_movingWall2.updateModelMatrix();
	stage3_movingWall2.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(stage3_movingWall2.modelMatrix));
	stage3_movingWall2.draw(36, 0);
	stage3_movingWall2.DeleteBuffer();

	Cube stage3_movingWall3;
	stage3_movingWall3.position = glm::vec3(1.4f, 0.7f, -15.5f + wallPosZ[2]);
	stage3_movingWall3.scale = glm::vec3(0.5f, 1.2f, 0.2f);
	stage3_movingWall3.rotationAngle = 0.0f;
	stage3_movingWall3.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	stage3_movingWall3.updateModelMatrix();
	stage3_movingWall3.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(stage3_movingWall3.modelMatrix));
	stage3_movingWall3.draw(36, 0);
	stage3_movingWall3.DeleteBuffer();

	Cube stage3_movingWall4;
	stage3_movingWall4.position = glm::vec3(-0.7f, 0.7f, -21.5f + wallPosZ[3]);
	stage3_movingWall4.scale = glm::vec3(0.5f, 1.2f, 0.2f);
	stage3_movingWall4.rotationAngle = 0.0f;
	stage3_movingWall4.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	stage3_movingWall4.updateModelMatrix();
	stage3_movingWall4.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(stage3_movingWall4.modelMatrix));
	stage3_movingWall4.draw(36, 0);
	stage3_movingWall4.DeleteBuffer();

	Cube stage3_movingWall5;
	stage3_movingWall5.position = glm::vec3(-1.4f, 0.7f, -23.5f + wallPosZ[4]);
	stage3_movingWall5.scale = glm::vec3(0.5f, 1.2f, 0.2f);
	stage3_movingWall5.rotationAngle = 0.0f;
	stage3_movingWall5.rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	stage3_movingWall5.updateModelMatrix();
	stage3_movingWall5.updateBounds();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(stage3_movingWall5.modelMatrix));
	stage3_movingWall5.draw(36, 0);
	stage3_movingWall5.DeleteBuffer();

	glUniform1i(glGetUniformLocation(shaderProgramID, "useTexture"), 0);
}

GLvoid Renderer::update() {
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	//floorPosZ += floorMoveSpeed * deltaTime;
	//if (floorPosZ > 1.5f) {
	//	floorMoveSpeed *= -1;
	//}
	//if (floorPosZ < -1.5f) {
	//	floorMoveSpeed *= -1;
	//}
	//
	//float maxZ[] = { 5.0f, 7.0f, 9.0f, 3.0f, 1.0f };
	//float minZ[] = { -5.0f, -3.0f, -1.0f, -7.0f, -9.0f };

	//for (int i = 0; i < 5; i++) {
	//	wallPosZ[i] += 2 * wallMoveSpeeds[i] * deltaTime;

	//	// 경계값 체크 및 wallMoveSpeed 반전
	//	if (wallPosZ[i] > maxZ[i] || wallPosZ[i] < minZ[i]) {
	//		wallMoveSpeeds[i] *= -1;
	//	}
	//}

	//for (int i = 0; i < missileYs.size(); i++) {
	//	missileYs[i] -= missileSpeed * deltaTime;  // 각 미사일의 위치 갱신
	//	if (missileYs[i] < 0.3f) {  // 범위 아래로 내려가면 새 위치 설정
	//		missileYs[i] = dist(rng);
	//	}

	//	// x, z 위치 계산
	//	int xPositionIndex = i % 3; // 3개의 x축 위치 중 하나 선택
	//	float xPosition = xPositionIndex == 0 ? -2 : xPositionIndex == 1 ? 0 : 2; // x축 위치 설정
	//	float zPosition = -28.5 + (i / 3 * -4); // z축 위치 설정
	//	if (i % 3 == 1) {
	//		zPosition = -28.5 + (i / 3 * -4) - 2;
	//	}

	//	RenderMissile(xPosition, missileYs[i], zPosition);
	//}

	//몬스터 이동
	enemyLegRotationAngle += enemyLegRotationRate * deltaTime;
	if (enemyLegRotationAngle > 30.0f) {
		enemyLegRotationAngle = 30.0f;
		enemyLegRotationRate = -enemyLegRotationRate;
	}
	else if (enemyLegRotationAngle < -30.0f) {
		enemyLegRotationAngle = -30.0f;
		enemyLegRotationRate = -enemyLegRotationRate;
	}

	enemyArmRotationAngle += enemyArmRotationRate * deltaTime;
	if (enemyArmRotationAngle > 30.0f) {
		enemyArmRotationAngle = 30.0f;
		enemyArmRotationRate = -enemyArmRotationRate;
	}
	else if (enemyArmRotationAngle < -30.0f) {
		enemyArmRotationAngle = -30.0f;
		enemyArmRotationRate = -enemyArmRotationRate;
	}
	

}

