#include "process.h"
#include "logger.h"
#include "counter.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

// Функция для запуска процессов
void spawn_processes() {
    static std::atomic<bool> copy_running(false);

    if (copy_running.load()) {
        log_message("Previous copy is still running. Skipping new process creation.");
        return;
    }

    copy_running.store(true);

    std::thread([]() {
        pid_t pid = fork();
        if (pid == 0) {
            log_message("Child process 1 started. PID: " + std::to_string(getpid()));
            counter.fetch_add(10); 
            log_message("Child process 1 exited. Counter: " + std::to_string(counter.load()));
            exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            log_message("Child process 2 started. PID: " + std::to_string(getpid()));
            int old_value = counter.fetch_add(counter.load()); 
            std::this_thread::sleep_for(std::chrono::seconds(2));
            counter.fetch_sub(old_value); 
            log_message("Child process 2 exited. Counter: " + std::to_string(counter.load()));
            exit(0);
        }

        copy_running.store(false);
    }).detach();
}
