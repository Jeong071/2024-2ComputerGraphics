#pragma once

class Renderer {
public:

	Renderer();
	~Renderer();

	static void InitializeTextures();

	static GLvoid RenderScene();
	static void CreateShader();

	static void RenderEnemy();
	static void RenderStage1();
	static void RenderStage2();
	static void RenderStage3();
	static void RenderSkyBox();

	static void ProcessObjCubeCollision();
	static void ProcessCollision();
	static bool CheckCollision(Cube& b);
	static bool IsInCube(Cube& b);
private:
	static std::vector<GLuint> textureIDs;
	static std::vector<Cube> cubes;
	static std::vector<Cube> objCubes;
	static GLuint shaderProgramID;
};
