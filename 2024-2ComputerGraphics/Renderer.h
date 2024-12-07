#pragma once

class Renderer {
public:
	Renderer();
	~Renderer();

	static GLvoid RenderScene();
	static void CreateShader();

	static void RenderStage1();
	static void RenderStage2();
	static void RenderStage3();

private:
	std::vector<Cube> cubes;
	static GLuint shaderProgramID;
};
