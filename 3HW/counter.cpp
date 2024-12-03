#include "counter.h"
#include <thread>
#include <chrono>
#include <iostream>

std::atomic<int> counter(0);

// Запуск таймера для увеличения счетчика
void start_counter_increment() {
    std::thread([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            ++counter;
        }
    }).detach();
}
