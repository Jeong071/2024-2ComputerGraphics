#pragma once
class Cube {
public:
    unsigned int VAO, VBO, EBO;
    glm::vec3 position, scale;
    float rotationAngle;
    glm::vec3 rotationAxis;
    glm::vec3 minPoint, maxPoint; // 두 점
    glm::mat4 modelMatrix;

    Cube();
    ~Cube();


        void draw();
        void updateModelMatrix(); // 모델 행렬 업데이트
        void updateBounds();
        void DeleteBuffer();

private:
    void setupMesh();
};

