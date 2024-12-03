#include "logger.h"
#include "counter.h"
#include "process.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <unistd.h> // Для getpid()

int main() {
    // Инициализация лог-файла
    init_log_file("log.txt");
    log_message("Main process started. PID: " + std::to_string(getpid()));

    // Запуск таймера для счетчика
    start_counter_increment();

    // Таймер записи в лог
    std::thread([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            log_message("Counter: " + std::to_string(counter.load()));
        }
    }).detach();

    // Таймер для запуска процессов
    std::thread([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            spawn_processes();
        }
    }).detach();

    // Ввод пользователя для изменения значения счетчика
    std::string input;
    while (true) {
        std::cout << "Enter new counter value: ";
        std::cin >> input;
        try {
            int new_value = std::stoi(input);
            counter.store(new_value);
            log_message("Counter set to " + std::to_string(new_value));
        } catch (...) {
            std::cerr << "Invalid input." << std::endl;
        }
    }

    close_log_file();
    return 0;
}
