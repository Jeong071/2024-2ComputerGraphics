#pragma once
class Timer {
public:
    explicit Timer(int interval);
    void start();

private:
    int interval_;
    static void TimerFunc(int value);
};
