#include "pch.h"
#include "Timer.h"

Timer::Timer(int interval) : interval_(interval) {}

void Timer::start() {
    glutTimerFunc(interval_, TimerFunc, 0);
}

void Timer::TimerFunc(int value) {
    glutTimerFunc(20, TimerFunc, 0);
    glutPostRedisplay();
}