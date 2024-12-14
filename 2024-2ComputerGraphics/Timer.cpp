#include "pch.h"
#include "Timer.h"

void Timer::UpdateFPS() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    frameCount++;

    // 1초(1000 밀리초)가 지났는지 확인
    if (currentTime - previousTime > 1000) {
        fps = frameCount * 1000.0f / (currentTime - previousTime);
        previousTime = currentTime;
        frameCount = 0;

        // FPS를 포함한 새로운 창 제목 생성
        std::stringstream ss;
        ss << "OpenGL FPS: " << fps;
        std::string title = ss.str();

        // 창 제목 업데이트
        glutSetWindowTitle(title.c_str());
    }
}