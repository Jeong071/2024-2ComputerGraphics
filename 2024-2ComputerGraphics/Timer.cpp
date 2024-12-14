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
Timer::Timer(int interval) : interval_(interval) {}

void Timer::start() {
    glutTimerFunc(interval_, TimerFunc, 0);
}

void Timer::TimerFunc(int value) {
    glutTimerFunc(20, TimerFunc, 0);
    glutPostRedisplay();

}