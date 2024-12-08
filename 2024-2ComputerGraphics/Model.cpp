#include "pch.h"
#include "Model.h"
//.obj �Ľ� ��  unordered_map���� �� ����

Model::Model()
{
    
}

Model::~Model()
{
}

void Model::ParseObj(const std::string& objFile)
{
    std::ifstream obj(objFile);
    if (!obj.is_open()) {
        std::cerr << "obj���� ���� ����" << std::endl;
        return;
    }

    
    std::string line;
    while (std::getline(obj, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;

        if (word == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            mPlayerVertices.push_back(vertex);
        }
        else if (word == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            mPlayerVertexNormals.push_back(normal);
        }
        else if (word == "vt") {
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            mPlayerVertexTextures.push_back(texCoord);
        }
        else if (word == "f") {
            // �ﰢ�� �� �Ľ�
            std::vector<Face> faceVertices;
            std::string vertexStr;

            // �ﰢ���̹Ƿ� ��Ȯ�� 3���� ���� ������ �־�� ��
            for (int i = 0; i < 3; ++i) {
                if (!(iss >> vertexStr)) {
                    // ���� ó��: �ﰢ���� �ʿ��� ���� ������ ������
                    throw std::runtime_error("Invalid face definition. Expected 3 vertices.");
                }

                // '/'�� �������� ��ü�Ͽ� v, vt, vn �ε��� �и�
                std::replace(vertexStr.begin(), vertexStr.end(), '/', ' ');
                std::istringstream viss(vertexStr);
                Face face = { 0, 0, 0 };

                // v, vt, vn �ε��� �Ľ�
                viss >> face.vertexIndex;
                if (!(viss >> face.textureIndex)) {
                    face.textureIndex = -1; // �ؽ�ó �ε����� ���� ��� -1�� ����
                }
                if (!(viss >> face.normalIndex)) {
                    face.normalIndex = -1; // ��� �ε����� ���� ��� -1�� ����
                }

                // OBJ �ε����� 1���� �����ϹǷ� 0 ������� ��ȯ
                face.vertexIndex = (face.vertexIndex > 0) ? (face.vertexIndex - 1) : (mPlayerVertices.size() + face.vertexIndex);
                face.textureIndex = (face.textureIndex > 0) ? (face.textureIndex - 1) : ((face.textureIndex != -1) ? (mPlayerVertexTextures.size() + face.textureIndex) : -1);
                face.normalIndex = (face.normalIndex > 0) ? (face.normalIndex - 1) : ((face.normalIndex != -1) ? (mPlayerVertexNormals.size() + face.normalIndex) : -1);

                faceVertices.push_back(face);
            }

            // �ﰢ���̹Ƿ� �� Ʈ���̾ޱַ��̼� ���ʿ�, �ٷ� �� ����Ʈ�� �߰�
            if (faceVertices.size() != 3) {
                throw std::runtime_error("Non-triangle face detected after triangulation.");
            }

            mPlayerFaces.push_back(faceVertices[0]);
            mPlayerFaces.push_back(faceVertices[1]);
            mPlayerFaces.push_back(faceVertices[2]);
        }
        // �ʿ信 ���� �ٸ� ���λ� ó��
    }

    // �ε��� �迭 ����
    for (const auto& f : mPlayerFaces) {
        mPlayerIndex.push_back(f.vertexIndex);
        // �ؽ�ó�� ��� �ε����� �ʿ信 ���� �߰��� ó��
        // ��: mPlayerTexIndex.push_back(f.textureIndex);
        //     mPlayerNormalIndex.push_back(f.normalIndex);
    }

}

void Model::BindBuffer()
{
    glGenVertexArrays(1, &mPlayerVAO);
    glGenBuffers(3, mPlayerVBO); // VBO[0]: Positions, VBO[1]: Normals, VBO[2]: Texture Coordinates
    glGenBuffers(1, &mPlayerEBO);

    glBindVertexArray(mPlayerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mPlayerVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, mPlayerVertices.size() * sizeof(glm::vec3), mPlayerVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, mPlayerVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, mPlayerVertexNormals.size() * sizeof(glm::vec3), mPlayerVertexNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, mPlayerVBO[2]);
    if (!mPlayerVertexTextures.empty()) {
        glBufferData(GL_ARRAY_BUFFER, mPlayerVertexTextures.size() * sizeof(glm::vec2), mPlayerVertexTextures.data(), GL_STATIC_DRAW);
      
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(2);
    }
    else {
        glDisableVertexAttribArray(2);
    }

    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPlayerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPlayerIndex.size() * sizeof(unsigned int), mPlayerIndex.data(), GL_STATIC_DRAW);

   
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

GLuint Model::GetVAO(std::string name)
{
    if (name == "Player") {
        return mPlayerVAO;
    }
}

GLuint* Model::GetVBO(std::string name)
{
    if (name == "Player") {
        return mPlayerVBO;
    }
}


void Model::RenderPlayer()
{
    glBindVertexArray(mPlayerVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mPlayerIndex.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Model::ReleaseBuffer()
{
    glDeleteVertexArrays(1, &mPlayerVAO);
    glDeleteBuffers(3, mPlayerVBO);
    glDeleteBuffers(1, &mPlayerEBO);
}

//������ ��� �Լ�
void Model::PrintInfo()
{
    for (auto& a : mPlayerVertices) {
        //std::cout << a[0] << "," << a[1] << "," << a[2] << std::endl;
    }

    std::cout << "========================" << std::endl;
    for (auto& a : mPlayerVertexNormals) {
        //std::cout << a[0] << "," << a[1] << "," << a[2] << std::endl;
    }
    std::cout << "========================" << std::endl;
    for (auto& a : mPlayerVertexTextures) {
        //std::cout << a[0] << "," << a[1]  << std::endl;
    }
    int cnt = 0;
    std::cout << "========================" << std::endl;
    for (auto& a : mPlayerFaces) {
        cnt++;
        //std::cout << a.vertexIndex << "," << a.normalIndex << "," << a.textureIndex << std::endl;
        if (cnt == 20) break;
    }
    
}
