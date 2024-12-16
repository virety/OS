#include <iostream>      
#include <thread>    
#include <cstdlib>       
#include <ctime>    
#include <unistd.h>    
#include <fcntl.h>   
#include <sys/stat.h>  

int main() {
    const std::string device = "/tmp/virtual_serial"; // Путь к виртуальному последовательному порту
    std::srand(std::time(nullptr));
    mkfifo(device.c_str(), 0666);
    int fifo = open(device.c_str(), O_WRONLY);
    // Основной цикл симуляции
    while (true) {
        // Генерируем случайную температуру (15.0°C - 30.0°C)
        double temperature = 15.0 + (std::rand() % 150) / 10.0;
        std::string temp_str = std::to_string(temperature) + "\n";
        write(fifo, temp_str.c_str(), temp_str.size());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    close(fifo);
    return 0;
}