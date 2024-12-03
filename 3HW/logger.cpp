#include "logger.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>

std::ofstream log_file;
std::mutex log_mutex;

// Получение текущего времени в строковом формате
std::string current_time() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;
    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &now_time);
#else
    localtime_r(&now_time, &tm_now); 
#endif
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_now);
    return std::string(buffer) + "." + std::to_string(ms.count());
}

// Инициализация лог-файла
void init_log_file(const std::string& filename) {
    log_file.open(filename, std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        exit(1);
    }
}

// Запись сообщения в лог
void log_message(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    log_file << current_time() << " " << message << std::endl;
}

// Закрытие лог-файла
void close_log_file() {
    if (log_file.is_open()) {
        log_file.close();
    }
}
