#pragma once

class Renderer {
public:

	Renderer();
	~Renderer();

	static void InitializeTextures();

	static void RenderScene();
	static void CreateShader();

	static void RenderEnemy();
	static void RenderStage1();
	static void RenderStage2();

	static void RenderStage3();
	static void RenderSkyBox();
	static void RenderStage3();	
	static void RenderMissile(float startX, float startY, float startz);
	static void RenderObstacle();
	static void update();




	static void ProcessCollision();
	static bool CheckCollision(Cube& b);


	static GLvoid RenderMainScene();
	static GLvoid RenderPlayScene();

	static std::vector<Cube> GetCubeIdx() { return objCubes; }
private:

	static std::vector<GLuint> textureIDs;
	static std::vector<Cube> cubes;
	static std::vector<Cube> objCubes;

	std::vector<Cube> missiles;
	std::vector<Cube> cubes;

	static std::vector<GLuint> textureIDs;

	static GLuint shaderProgramID;

};
