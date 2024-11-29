#pragma once
class Renderer {
public:
	Renderer();
	~Renderer();

	static GLvoid RenderScene();

private:
	static GLuint shaderProgramID;
};

