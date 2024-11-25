#pragma once
class Shader
{
public:
	Shader();
	~Shader();
	GLuint CreateVertexShader(const char* ShaderFile);
	GLuint CreateFragmentShader(const char* ShaderFile);
	GLuint CreateShaderProgram(GLuint vShader, GLuint fShader);
	char* filetobuf(const char* file);

};

