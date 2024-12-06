#pragma once

class Renderer {
public:
	Renderer();
	~Renderer();

	static GLvoid RenderScene();
	static void CreateShader();

private:
	static GLuint shaderProgramID;
};
