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

private:
	static std::vector<GLuint> textureIDs;
	std::vector<Cube> cubes;
	static GLuint shaderProgramID;
};
