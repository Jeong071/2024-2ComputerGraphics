#include "pch.h"
#include "Timer.h"

void Timer::UpdateFPS() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    frameCount++;

    // 1��(1000 �и���)�� �������� Ȯ��
    if (currentTime - previousTime > 1000) {
        fps = frameCount * 1000.0f / (currentTime - previousTime);
        previousTime = currentTime;
        frameCount = 0;

        // FPS�� ������ ���ο� â ���� ����
        std::stringstream ss;
        ss << "OpenGL FPS: " << fps;
        std::string title = ss.str();

        // â ���� ������Ʈ
        glutSetWindowTitle(title.c_str());
    }
}