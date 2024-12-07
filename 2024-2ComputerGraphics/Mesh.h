#pragma once
class Cube {
    public:
        unsigned int VAO, VBO, EBO;

        Cube();
        ~Cube();

        void draw();
        void DeleteBuffer();
};

