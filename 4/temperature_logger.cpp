#include <iostream>      
#include <fstream>   
#include <sstream>  
#include <vector>    
#include <deque>      
#include <string>    
#include <chrono>     
#include <ctime>      
#include <iomanip>    
#include <thread>        // Для многопоточности
#include <mutex>         // Для синхронизации потоков
#include <termios.h>     // Для работы с последовательным портом
#include <fcntl.h>    
#include <unistd.h>    

// Названия лог-файлов
const std::string LOG_ALL = "log_all_measurements.log";      
const std::string LOG_HOURLY = "log_hourly_averages.log";  
const std::string LOG_DAILY = "log_daily_averages.log";    

// Мьютекс для синхронизации
std::mutex log_mutex;

// Очередь для хранения всех измерений
std::deque<std::pair<std::time_t, double>> measurements;

// Функция записи в лог-файл
void write_log(const std::string& filename, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex); // Защита от одновременной записи
    std::ofstream log_file(filename, std::ios::app);
    if (log_file) {
        log_file << message << std::endl;
    }
}

// Очистка устаревших записей
void cleanup_logs() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    // Очистка лога всех измерений
    auto cutoff_24h = now_time - 24 * 3600; 
    std::ifstream infile(LOG_ALL);
    std::ofstream temp_file("temp_all.log");
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::time_t timestamp;
        iss >> timestamp;
        if (timestamp > cutoff_24h) {
            temp_file << line << std::endl;
        }
    }
    infile.close();
    temp_file.close();
    std::rename("temp_all.log", LOG_ALL.c_str());
}

// Вычисление среднего значения
double calculate_average(const std::vector<double>& values) {
    double sum = 0;
    for (double v : values) {
        sum += v;
    }
    return values.empty() ? 0 : sum / values.size();
}

// Поток обработки данных (почасовые и дневные средние)
void process_measurements() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(1)); // Раз в час
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        auto start_time = now_time - 3600; // Граница текущего часа

        // Почасовые температуры
        std::vector<double> hourly_temps;
        {
            std::lock_guard<std::mutex> lock(log_mutex);
            for (const auto& [timestamp, temp] : measurements) {
                if (timestamp >= start_time) {
                    hourly_temps.push_back(temp);
                }
            }
        }

        // Среднее за час
        double hourly_avg = calculate_average(hourly_temps);
        std::ostringstream oss;
        oss << now_time << " " << hourly_avg;
        write_log(LOG_HOURLY, oss.str()); // Запись почасового среднего
    }
}

// Чтение данных с устройства
void read_temperature_from_device(const std::string& device) {
    // Открываем FIFO как обычный файл
    int fifo = open(device.c_str(), O_RDONLY);
    if (fifo < 0) {
        perror("Error opening FIFO");
        return;
    }

    char buffer[256];
    while (true) {
        int n = read(fifo, &buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0'; // Завершаем строку
            double temperature = std::stod(buffer); // Преобразуем строку в число
            auto now = std::chrono::system_clock::now();
            auto now_time = std::chrono::system_clock::to_time_t(now);
            {
                std::lock_guard<std::mutex> lock(log_mutex);
                measurements.emplace_back(now_time, temperature);
            }
            std::ostringstream oss;
            oss << now_time << " " << temperature;
            write_log(LOG_ALL, oss.str());
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Пауза
    }

    close(fifo); // Закрываем FIFO
}

int main() {
    std::thread hourly_thread(process_measurements); // Поток обработки почасовых данных

    const std::string device = "/tmp/virtual_serial"; // Указание устройства
    read_temperature_from_device(device); // Чтение данных

    hourly_thread.join(); // Ожидание завершения потока
    return 0;
}
