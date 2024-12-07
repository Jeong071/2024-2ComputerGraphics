#pragma once
struct Face {
	int vertexIndex;
	int textureIndex;
	int normalIndex;
};

class Model
{
public:
	Model();
	~Model();

	void ParseObj(const std::string& objFile);
	void BindBuffer();

	GLuint GetVAO(std::string name);
	GLuint* GetVBO(std::string name);
	int GetVertexCount() { return mPlayerVertices.size(); }

	void RenderPlayer();
	void ReleaseBuffer();

	void PrintInfo(); //저장 여부 디버그용 함수

private:

	GLuint mPlayerVAO;
	GLuint mPlayerVBO[3]; //0 : vertex, 1 : vertexNormal, 2 : vertexTexture 
	GLuint mPlayerEBO;

	std::vector<glm::vec3> mPlayerVertices;
	std::vector<glm::vec3> mPlayerVertexNormals;
	std::vector<glm::vec2> mPlayerVertexTextures;
	std::vector<Face> mPlayerFaces;
	std::vector<int> mPlayerIndex;

};

